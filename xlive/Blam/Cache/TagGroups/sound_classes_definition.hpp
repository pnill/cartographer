#pragma once
#include "Blam/Cache/DataTypes/BlamDataTypes.h"
#include "Blam/Cache/TagGroups.hpp"

#include <wtypes.h>

#pragma pack(push,1)
struct s_sound_classes_block : TagGroup<'sncl'>
{
    struct sound_class_block
    {
        /// maximum number of sounds playing per individual sound tag
        short maxSoundsPerTag116;
        /// maximum number of sounds of this type playing on an object
        short maxSoundsPerObject116;
        /// replaces other instances after this many milliseconds
        int preemptionTimeMs;

        enum InternalFlags : short
        {
            Valid = 0x1,
            IsSpeech = 0x2,
            Scripted = 0x4,
            StopsWithObject = 0x8,
            Unused = 0x10,
            ValidDopplerFactor = 0x20,
            ValidObstructionFactor = 0x40,
            Multilingual = 0x80,
        };
        InternalFlags internalFlags;

        enum Flags : short
        {
            PlaysDuringPause = 0x1,
            DryStereoMix = 0x2,
            NoObjectObstruction = 0x4,
            UseCenterSpeakerUnspatialized = 0x8,
            SendmonoToLfe = 0x10,
            Deterministic = 0x20,
            UseHugeTransmission = 0x40,
            AlwaysUseSpeakers = 0x80,
            DontStripFromMainMenu = 0x100,
            IgnoreStereoHeadroom = 0x200,
            LoopFadeOutIsLinear = 0x400,
            StopWhenObjectDies = 0x800,
            AllowCacheFileEditing = 0x1000,
        };
        Flags flags;
        short priority;

        enum CacheMissMode : short
        {
            Discard = 0,
            Postpone = 1,
        };
        CacheMissMode cacheMissMode;
        /// how much reverb applies to this sound class
        float reverbGainDB;
        float overrideSpeakerGainDB;
        real_bounds distanceBounds;
        /* edit */
        real_bounds gainBoundsDB;
        float cutsceneDuckingDB;
        float cutsceneDuckingFadeInTimeSeconds;
        /// how long this lasts after the cutscene ends
        float cutsceneDuckingSustainTimeSeconds;
        float cutsceneDuckingFadeOutTimeSeconds;
        float scriptedDialogDuckingDB;
        float scriptedDialogDuckingFadeInTimeSeconds;
        /// how long this lasts after the scripted dialog ends
        float scriptedDialogDuckingSustainTimeSeconds;
        float scriptedDialogDuckingFadeOutTimeSeconds;
        float dopplerFactor;

        enum StereoPlaybackType : byte
        {
            FirstPerson = 0,
            Ambient = 1,
        };
        StereoPlaybackType stereoPlaybackType;
        byte padding15[3];
        float transmissionMultiplier;
        float obstructionMaxBend;
        float occlusionMaxBend;
    };
    TAG_GROUP_SIZE_ASSERT(sound_class_block, 92);
    tag_block<sound_class_block> soundClasses;
    PAD(4);
};
TAG_GROUP_SIZE_ASSERT(s_sound_classes_block, 12);
#pragma pack(pop)