#pragma once

#include "../UEGameProfile.hpp"
using namespace UEMemory;

class PUBGProfile : public IGameProfile
{
public:
    PUBGProfile() = default;

    std::string GetAppName() const override
    {
        return "PUBG";
    }

    std::vector<std::string> GetAppIDs() const override
    {
        return {
            "com.tencent.ig",
            "com.rekoo.pubgm",
            "com.pubg.imobile",
            "com.pubg.krmobile",
            "com.vng.pubgmobile",
        };
    }

    bool isUsingCasePreservingName() const override
    {
        return false;
    }

    bool IsUsingFNamePool() const override
    {
        return false;
    }

    bool isUsingOutlineNumberName() const override
    {
        return false;
    }

    uintptr_t GetGUObjectArrayPtr() const override
    {
        std::string ida_pattern = "80 B9 ? ? ? ? ? ? ? 91 ? ? 40 F9 ? 03 80 52";
        const int step = 2;

        auto text_seg = GetExecutableInfo().getSection("__TEXT", "__text");
        uintptr_t ins = KittyScanner::findIdaPatternFirst(text_seg.start, text_seg.end, ida_pattern);
        if (ins != 0)
        {
            return Arm64::Decode_ADRP_ADD(ins + step);
        }

        return 0;
    }

    uintptr_t GetNamesPtr() const override
    {
        std::string ida_pattern =  "ff c3 01 91 c0 03 5f d6 ? ? ? ? ? ? ? 91 ? ? ? 94 ? ? ? 34 ff ? ? ? ? ? ? ? ? ? ? 91";
        const int step = 0x1c;

        auto text_seg = GetExecutableInfo().getSection("__TEXT", "__text");
        uintptr_t ins = KittyScanner::findIdaPatternFirst(text_seg.start, text_seg.end, ida_pattern);
        if (ins == 0)
            return 0;

        uintptr_t param_1 = Arm64::Decode_ADRP_ADD(ins + step);
        if (param_1 == 0)
            return 0;

        int64_t var_2;
        int64_t var_5[16];

        var_2 = (vm_rpm_ptr<int32_t>((void *)(param_1)) - 100) / 3u;
        var_5[(uint32_t)(var_2 - 1)] = vm_rpm_ptr<int64_t>((void *)(param_1 + 8));

        while (var_2 - 2 >= 0)
        {
            var_5[(uint32_t)(var_2 - 2)] = vm_rpm_ptr<int64_t>((void *)(var_5[var_2 - 1]));
            --var_2;
        }

        return var_5[0];
    }

    UE_Offsets *GetOffsets() const override
    {
        static UE_Offsets offsets = UE_DefaultOffsets::UE4_18_19(isUsingCasePreservingName());

        static bool once = false;
        if (!once)
        {
            once = true;
            offsets.FNameEntry.Index = sizeof(void *);
            offsets.FNameEntry.Name = sizeof(void *) + sizeof(int32_t);
        }

        return &offsets;
    }
};
