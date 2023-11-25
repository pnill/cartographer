#include "stdafx.h"
#include "KantTesting.h"

#include "H2MOD/Modules/Input/KeyboardInput.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"
#include "Util/Hooks/Hook.h"

namespace KantTesting
{
	void MapLoad()
	{
	}

    static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";


    static inline bool is_base64(BYTE c) {
        return (isalnum(c) || (c == '+') || (c == '/'));
    }

    std::string base64_encode(BYTE const* buf, unsigned int bufLen) {
        std::string ret;
        int i = 0;
        int j = 0;
        BYTE char_array_3[3];
        BYTE char_array_4[4];

        while (bufLen--) {
            char_array_3[i++] = *(buf++);
            if (i == 3) {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for (i = 0; (i < 4); i++)
                    ret += base64_chars[char_array_4[i]];
                i = 0;
            }
        }

        if (i)
        {
            for (j = i; j < 3; j++)
                char_array_3[j] = '\0';

            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (j = 0; (j < i + 1); j++)
                ret += base64_chars[char_array_4[j]];

            while ((i++ < 3))
                ret += '=';
        }

        return ret;
    }

    std::vector<BYTE> base64_decode(std::string const& encoded_string) {
        int in_len = encoded_string.size();
        int i = 0;
        int j = 0;
        int in_ = 0;
        BYTE char_array_4[4], char_array_3[3];
        std::vector<BYTE> ret;

        while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
            char_array_4[i++] = encoded_string[in_]; in_++;
            if (i == 4) {
                for (i = 0; i < 4; i++)
                    char_array_4[i] = base64_chars.find(char_array_4[i]);

                char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

                for (i = 0; (i < 3); i++)
                    ret.push_back(char_array_3[i]);
                i = 0;
            }
        }

        if (i) {
            for (j = i; j < 4; j++)
                char_array_4[j] = 0;

            for (j = 0; j < 4; j++)
                char_array_4[j] = base64_chars.find(char_array_4[j]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (j = 0; (j < i - 1); j++) ret.push_back(char_array_3[j]);
        }

        return ret;
    }

	typedef void(__cdecl t_join_game)(int8* session_info);
	t_join_game* p_join_game;
    std::string lolz;
	void __cdecl c_join_game(int8* session_info)
	{

        if (lolz.empty())
        {
            auto encoded = base64_encode(reinterpret_cast<byte const*>(session_info), 0xA1C);
            lolz = std::string(encoded);
            LOG_INFO_GAME(encoded);
        }
        p_join_game(session_info);
        //auto decoded = base64_decode(lolz);
		//p_join_game(reinterpret_cast<int8*>(decoded.data()));
	}

    void hot_join()
	{
        addDebugText("Hot Joining a lobby..");
        auto decoded = base64_decode(lolz);
        p_join_game(reinterpret_cast<int8*>(decoded.data()));
	}

    int klk = VK_DELETE;
	void Initialize()
	{
		if (ENABLEKANTTEST) {
			p_join_game = Memory::GetAddress<t_join_game*>(0x2160E2);
			PatchCall(Memory::GetAddress(0x21731C), c_join_game);
            KeyboardInput::RegisterHotkey(&klk, []()
                {
                    hot_join();
                });
		//	if (!Memory::isDedicatedServer())
			//{
			//tags::on_map_load(MapLoad);
		//	}
		}
	}
}
