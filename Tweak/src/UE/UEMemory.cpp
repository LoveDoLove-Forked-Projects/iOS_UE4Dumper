#include "UEMemory.hpp"

namespace UEMemory
{
    KittyPtrValidator kPtrValidator;

    bool vm_rpm_ptr(const void *address, void *result, size_t len)
    {
        if (!kPtrValidator.isPtrReadable(address))
            return false;

        // faster but will crash on any invalid address
#ifdef RPM_USE_MEMCPY
        return memcpy(result, address, len) != nullptr;
#else

        vm_size_t outSize = 0;
        kern_return_t kret = vm_read_overwrite(mach_task_self(), (vm_address_t)address,
                                               (vm_size_t)len, (vm_address_t)result, &outSize);

        return kret == 0 && outSize == len;

#endif
    }

    std::string vm_rpm_str(const void *address, size_t max_len)
    {
#ifdef RPM_USE_MEMCPY
        if (kPtrValidator.isPtrReadable(address))
        {
            const char *chars = (const char *)address;
            std::string str = "";
            for (size_t i = 0; i < max_len; i++)
            {
                if (chars[i] == '\0')
                    break;

                str.push_back(chars[i]);
            }
            return str;
        }
        return "";
#else
        std::vector<char> chars(max_len, '\0');
        if (!vm_rpm_ptr(address, chars.data(), max_len))
            return "";

        std::string str = "";
        for (size_t i = 0; i < chars.size(); i++)
        {
            if (chars[i] == '\0')
                break;

            str.push_back(chars[i]);
        }

        chars.clear();
        chars.shrink_to_fit();

        if ((int)str[0] == 0 && str.size() == 1)
            return "";

        return str;
#endif
    }

    std::wstring vm_rpm_strw(const void *address, size_t max_len)
    {
#ifdef RPM_USE_MEMCPY
        if (kPtrValidator.isPtrReadable(address))
        {
            const wchar_t *chars = (const wchar_t *)address;
            std::wstring str = L"";
            for (size_t i = 0; i < max_len; i++)
            {
                if (chars[i] == L'\0')
                    break;

                str.push_back(chars[i]);
            }
            return str;
        }
        return L"";
#else
        std::vector<wchar_t> chars(max_len, '\0');
        if (!vm_rpm_ptr(address, chars.data(), max_len * 2))
            return L"";

        std::wstring str = L"";
        for (size_t i = 0; i < chars.size(); i++)
        {
            if (chars[i] == L'\0')
                break;

            str.push_back(chars[i]);
        }

        chars.clear();
        chars.shrink_to_fit();

        if ((int)str[0] == 0 && str.size() == 1)
            return L"";

        return str;
#endif
    }

    uintptr_t FindAlignedPointerRefrence(uintptr_t start, size_t range, uintptr_t ptr)
    {
        if (start == 0 || start != GetPtrAlignedOf(start))
            return 0;

        if (range < sizeof(void *) || range != GetPtrAlignedOf(range))
            return 0;

        for (size_t i = 0; (i + sizeof(void *)) <= range; i += sizeof(void *))
        {
            uintptr_t val = vm_rpm_ptr<uintptr_t>((void *)(start + i));
            if (val == ptr) return (start + i);
        }
        return 0;
    }

    namespace Arm64
    {
        uintptr_t DecodeADRL(uintptr_t adrp_address, uint32_t imm_insn_offset)
        {
            if (adrp_address == 0) return 0;

            uint32_t adrp_insn = vm_rpm_ptr<uint32_t>((void *)(adrp_address));
            if (adrp_insn == 0)
                return 0;

            KittyInsnArm64 adrp_decoded = KittyArm64::decodeInsn(adrp_insn, adrp_address);
            if (adrp_decoded.type != EKittyInsnTypeArm64::ADR && adrp_decoded.type != EKittyInsnTypeArm64::ADRP)
                return 0;

            if (imm_insn_offset == 0)
            {
                // scan next 8 instructions
                // adrp rd == imm rn
                for (int i = 1; i < 8; i++)
                {
                    uint32_t imm_insn = vm_rpm_ptr<uint32_t>((void *)(adrp_address + (i * 4)));
                    KittyInsnArm64 imm_decoded = KittyArm64::decodeInsn(imm_insn);
                    if (imm_decoded.isValid() && imm_decoded.immediate != 0 && adrp_decoded.rd == imm_decoded.rn)
                    {
                        return adrp_decoded.target + imm_decoded.immediate;
                    }
                }
            }
            else
            {
                uint32_t imm_insn = vm_rpm_ptr<uint32_t>((void *)(adrp_address + imm_insn_offset));
                if (imm_insn == 0)
                    return 0;

                KittyInsnArm64 imm_decoded = KittyArm64::decodeInsn(imm_insn);
                if (imm_decoded.isValid() && imm_decoded.immediate != 0)
                {
                    return adrp_decoded.target + imm_decoded.immediate;
                }
            }

            return 0;
        }
    }  // namespace Arm64

}  // namespace UEMemory

namespace IOUtils
{
    std::string remove_specials(std::string s)
    {
        for (size_t i = 0; i < s.size(); i++)
        {
            if (!((s[i] < 'A' || s[i] > 'Z') && (s[i] < 'a' || s[i] > 'z')))
                continue;

            if (!(s[i] < '0' || s[i] > '9'))
                continue;

            if (s[i] == '_')
                continue;

            s.erase(s.begin() + i);
            --i;
        }
        return s;
    }

    std::string replace_specials(std::string s, char c)
    {
        for (size_t i = 0; i < s.size(); i++)
        {
            if (!((s[i] < 'A' || s[i] > 'Z') && (s[i] < 'a' || s[i] > 'z')))
                continue;

            if (!(s[i] < '0' || s[i] > '9'))
                continue;

            if (s[i] == '_')
                continue;

            s[i] = c;
        }
        return s;
    }
}  // namespace IOUtils
