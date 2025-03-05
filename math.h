#include <cstdint>
#include <string>
#include <locale>
#include <vector>

namespace EngineUtils {

    class FName
    {
    public:
        static inline void* GNames = nullptr;

        uint32_t ComparisonIndex = 0;
        uint32_t Number = 0;

        inline uint32_t GetDisplayIndex() const noexcept
        {
            return ComparisonIndex;
        }

        inline bool operator==(const FName& Other) const noexcept
        {
            return ComparisonIndex == Other.ComparisonIndex && Number == Other.Number;
        }

        inline bool operator!=(const FName& Other) const noexcept
        {
            return !(*this == Other);
        }
    }; // Added semicolon here
}
