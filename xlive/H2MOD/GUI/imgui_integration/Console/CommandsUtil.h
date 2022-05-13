#pragma once

#include "imgui.h"
#include "CommandHandler.h"

struct StringLineHeader;
typedef int StringHeaderFlags;

#define MAX_CONSOLE_INPUT_BUFFER 256

// for redirecting output to either dedicated server/imgui console
class IOutput
{
public:
	virtual int Output(StringHeaderFlags flags, const char* fmt) = 0;
	virtual int OutputFmt(StringHeaderFlags flags, const char* fmt, ...) = 0;
};

static bool InputTextContainsCommandSubstring(const char* command, const char* input_text, bool test_only_first_token)
{
	char input_text_upper_temp[MAX_CONSOLE_INPUT_BUFFER];
	char comand_text_upper[MAX_CONSOLE_INPUT_BUFFER];

	strncpy(input_text_upper_temp, input_text, MAX_CONSOLE_INPUT_BUFFER - 1);
	input_text_upper_temp[MAX_CONSOLE_INPUT_BUFFER - 1] = '\0';

	strncpy(comand_text_upper, command, MAX_CONSOLE_INPUT_BUFFER - 1);
	comand_text_upper[MAX_CONSOLE_INPUT_BUFFER - 1] = '\0';

	_strupr(input_text_upper_temp);
	_strupr(comand_text_upper);

	char* next_token_to_test = NULL;
	char* text_to_test = input_text_upper_temp;

	while (text_to_test != NULL)
	{
		// first we get the next token to test
		// and remove the spaces in the buffer
		char* input_text_space = strstr(text_to_test, " ");
		if (input_text_space == NULL)
		{
			next_token_to_test = NULL;
		}
		else
		{
			// remove the space and test the next string after the space
			*input_text_space = '\0';
			input_text_space++;
			next_token_to_test = input_text_space;
		}

		if (strstr(comand_text_upper, text_to_test) != NULL)
			return true;

		if (test_only_first_token)
			return false;

		// at the end set the next substring to test from the input, if we have spaces in it
		text_to_test = next_token_to_test;

		// check if there's nothing after the space, if so return false, since we didn't find anything close
		if ((text_to_test - input_text_upper_temp) >= MAX_CONSOLE_INPUT_BUFFER)
		{
			return false;
		}
	}

	// if we got this far, no match was found
	return false;
}

template<typename T>
static inline bool tokenize(const char* str, size_t str_length, const char* delimiters, std::vector<T>& out, int tokenize_count = 0)
{
    out.clear();
    size_t beg, pos = 0;
    std::string _str(str, str_length);
    while ((beg = _str.find_first_not_of(delimiters, pos)), beg != std::string::npos)
    {
        pos = _str.find_first_of(delimiters, beg);
        out.push_back(_str.substr(beg, pos - beg));
        if (tokenize_count > 0 && out.size() == tokenize_count)
            break;
    }

    if (out.size() <= 0)
        return false;

    return true;
}

struct StringLineHeader
{
    size_t offset;
    size_t size;
    StringHeaderFlags flags;
};

enum StringHeaderFlags_
{
    StringFlag_None,
    StringFlag_History = 1 << 0, // when going through history with key up/key down, it'll be displayed in the input console
    StringFlag_CopyToClipboard = 1 << 1, // when set, it'll copy this string to clipboard, then unset the flag
};

// pretty much a circular buffer
// not thread-safe
class CircularStringBuffer
{
public:
    // initial buffer size
    unsigned int line_count;
    size_t max_buffer_size_per_line;

    size_t GetBufferSize() const { return line_count * max_buffer_size_per_line; }

    CircularStringBuffer(unsigned int _line_count, size_t _max_buffer_size_per_line)
    {
        line_count = _line_count;
        max_buffer_size_per_line = _max_buffer_size_per_line;

        // allocate once we know the size
        s = new char[GetBufferSize()];
        last_buffer_position = s;
        strings_header.reserve(line_count);
    };

    //CircularStringBuffer()
    //{
        // default 256 strings of 256 bytes each
        // to note default line count isn't that relevant when using dynamic string allocation from the buffer
        // what does dynamic string allocation mean in this context?
        // everytime someone tries to pass a string to this buffer
        // it'll allow the code to append it exactly after the NULL delimiter of the previous string
        // if the buffer is full, it'll override the first string, but im too lazy rn to fully implement and fix all edge cases this behaviour implies 
        // so we just use a static lenght buffer for each string, even if it sounds like wasted memory
    //}

    ~CircularStringBuffer()
    {
        delete[] s;
        s = NULL;
        last_buffer_position = NULL;
    }

    void Clear()
    {
        strings_header.clear();
        last_buffer_position = s;
        last_buffer_position[0] = '\0';
    }

    void StringBufferResize(int newLineCount, size_t newDefaultMaxBufferSizePerLine)
    {
        size_t oldBufferSize = GetBufferSize();

        line_count = newLineCount;
        max_buffer_size_per_line = newDefaultMaxBufferSizePerLine;

        size_t newBufferSize = GetBufferSize();
        IM_ASSERT(newBufferSize > oldBufferSize);
        size_t lastPosition = GetUsedBufferSize();
        char* new_buffer = (char*)IM_ALLOC(newBufferSize);
        if (s != NULL)
        {
            memcpy(new_buffer, s, GetBufferSize());
            IM_DELETE(s);
        }
        s = new_buffer;
        last_buffer_position = &s[lastPosition];
    }

    /// <summary>
    /// Add a string entry in the buffer
    /// </summary>
    /// <param name="source"></param>
    /// Source of the string
    /// <param name="characterCount"></param>
    /// Character count of the string
    void AddString(StringHeaderFlags flags, const char* source, size_t characterCount = 0)
    {
        if (characterCount == 0)
            characterCount = strnlen_s(source, max_buffer_size_per_line - 1);

        IM_ASSERT(characterCount < max_buffer_size_per_line - 1 || source[characterCount] == '\0');

        // TODO view comment in constructor
        //size_t sourceStringSize = characterCount + 1; // + 1 for the NULL

        char* destinationBuffer;
        size_t destinationBufferSize = GetNewlineBuffer(max_buffer_size_per_line, &destinationBuffer);
        // TODO view comment in constructor
        // size_t destinationBufferSize = GetNewlineBuffer(sourceStringSize, &destinationBuffer);

        if (destinationBufferSize > 0)
        {
            // position has been updated, copy the source string
            strncpy_s(destinationBuffer, destinationBufferSize, source, characterCount);
            destinationBuffer[characterCount] = '\0';

            strings_header.push_back(StringLineHeader{ (size_t)(destinationBuffer - s), destinationBufferSize, flags });
        }
        else
        {
            // log error
            // printf("AddString failed, destination buffer size < 0");
        }
    }

    void AddStringFmt(StringHeaderFlags flags, const char* fmt, ...)
    {
        va_list valist;
        va_start(valist, fmt);
        int buffer_size_needed = _vsnprintf(NULL, 0, fmt, valist) + 1;
        if (buffer_size_needed < max_buffer_size_per_line)
        {
            char* buffer = (char*)_malloca(max_buffer_size_per_line);
            int copied_characters = _vsnprintf(buffer, buffer_size_needed, fmt, valist);
            AddString(flags, buffer, copied_characters);
            _freea(buffer);
        }
        va_end(valist);
    }

	void AddStringFmt(StringHeaderFlags flags, const char* fmt, va_list valist)
	{
		int buffer_size_needed = _vsnprintf(NULL, 0, fmt, valist) + 1;
		if (buffer_size_needed < max_buffer_size_per_line)
		{
			char* buffer = (char*)_malloca(max_buffer_size_per_line);
			int copied_characters = _vsnprintf(buffer, buffer_size_needed, fmt, valist);
			AddString(flags, buffer, copied_characters);
			_freea(buffer);
		}
	}

    const char* GetStringAtIndex(int headerIndex) const
    {
        assert(headerIndex < GetStringHeaderSize());
        const StringLineHeader& string_header = GetHeader(headerIndex);
        return GetStringAtOffset(string_header.offset);
    }

    const char* GetStringAtOffset(size_t offset) const
    {
        return &s[offset];
    }

    const StringLineHeader& GetHeader(int headerIndex) const
    {
        return strings_header.at(headerIndex);
    }

    StringLineHeader& GetHeader(int headerIndex)
    {
        return strings_header.at(headerIndex);
    }

    size_t GetStringHeaderSize() const
    {
        return strings_header.size();
    }

private:
    // private data
    char* s; // buffer

    // buffer details
    // current position of the string
    char* last_buffer_position;
    // header for each line of characters
    std::vector<StringLineHeader> strings_header;

    /// <summary>
    /// Gets the size of the buffer and the pointer to the buffer itself
    /// where to copy the source string
    /// </summary>
    /// <param name="sourceStringSize"></param>
    /// size of the string about to get copied in the circular buffer
    /// <param name="outDestinationBuf"></param>
    /// pointer to destination buffer
    /// <returns></returns>
    size_t GetNewlineBuffer(size_t sourceStringBufferSize, char** outBufferPtr)
    {
        if (sourceStringBufferSize > GetBufferSize()) return 0; // we cannot copy this string, buffer too small

        // check if can fit the 'size' in current position remaining buffer
        bool remainingSpaceAvailable = (GetRemainingBufferSize() / sourceStringBufferSize) >= 1;
        if (remainingSpaceAvailable)
        {
            *outBufferPtr = last_buffer_position;
            // use current position as the buffer, there's enough space
            last_buffer_position += sourceStringBufferSize;
            return sourceStringBufferSize;
        }
        else
        {
            // then discard the old one
            last_buffer_position = s;

            auto OffsetInRange = [](size_t lower, size_t upper, size_t offset) -> bool
            {
                return lower <= offset && offset <= upper;
            };

            // clear string range, otherwise we might leave bad string_positions inside the container
            for (auto it = strings_header.begin(); it < strings_header.end(); )
            {
                if (OffsetInRange((size_t)(last_buffer_position - s), (size_t)((size_t)(last_buffer_position - s) + sourceStringBufferSize), it->offset))
                {
                    it = strings_header.erase(it);
                }
                else
                {
                    it++;
                }
            }

            // then attempt one more time
            return GetNewlineBuffer(sourceStringBufferSize, outBufferPtr);
        }
    }

    /// <summary>
    /// </summary>
    size_t GetRemainingBufferSize() const
    {
        return GetBufferSize() - GetUsedBufferSize();
    }

    /// <summary>
    /// </summary>
    size_t GetUsedBufferSize() const
    {
        return (size_t)(last_buffer_position - s);
    }
};