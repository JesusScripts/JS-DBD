#pragma once

#ifdef _KERNEL_MODE
inline namespace SKCRYPTUTILS
{
	// STRUCT TEMPLATE remove_reference
	template <class _Ty>
	struct remove_referencee {
		using type = _Ty;
	};

	template <class _Ty>
	struct remove_referencee<_Ty&> {
		using type = _Ty;
	};

	template <class _Ty>
	struct remove_referencee<_Ty&&> {
		using type = _Ty;
	};

	template <class _Ty>
	using remove_referencee_t = typename remove_referencee<_Ty>::type;

	// STRUCT TEMPLATE remove_const
	template <class _Ty>
	struct remove_constt { // remove top-level const qualifier
		using type = _Ty;
	};

	template <class _Ty>
	struct remove_constt<const _Ty> {
		using type = _Ty;
	};

	template <class _Ty>
	using remove_const_u = typename remove_constt<_Ty>::type;
}
#else
#include <type_traits>
namespace SKCRYPTUTILS {
	using std::remove_reference;
	using std::remove_const;
	using std::remove_reference_t;
	using std::remove_const_t;
}
#endif

inline namespace skc
{
	template<class _Ty>
	using clean_type = typename SKCRYPTUTILS::remove_const_t<SKCRYPTUTILS::remove_reference_t<_Ty>>;

	template <int _size, char _key1, char _key2, char _key3, typename T>
	class skCrypter
	{
	public:
		__forceinline constexpr skCrypter(T* data)
		{
			crypt(data);
		}

		__forceinline T* get()
		{
			return _storage;
		}

		__forceinline int size() // (w)char count
		{
			return _size;
		}

		__forceinline  char key()
		{
			return _key1;
		}

		__forceinline  T* encrypt()
		{
			if (!isEncrypted())
				crypt(_storage);

			return _storage;
		}

		__forceinline  T* decrypt()
		{
			if (isEncrypted())
				crypt(_storage);

			return _storage;
		}

		__forceinline bool isEncrypted()
		{
			return _storage[_size - 1] != 0;
		}

		__forceinline void clear() // set full storage to 0
		{
			for (int i = 0; i < _size; i++)
			{
				_storage[i] = 0;
			}
		}

		__forceinline operator T* ()
		{
			decrypt();

			return _storage;
		}

	private:
		__forceinline constexpr void crypt(T* data)
		{
			for (int i = 0; i < _size; i++)
			{
				_storage[i] = data[i] ^ (_key1 + i % (1 + _key2)) ^ _key3;
			}
		}

		T _storage[_size]{};
	};
}
constexpr char myCompileTimeKey[] = "OAWJRPSHSAFGASJCHASBKWRKASBV";
#define E(str) skCrypt_key(str, myCompileTimeKey[4], myCompileTimeKey[7], myCompileTimeKey[10])
#define _(str) skCrypt_key(str, myCompileTimeKey[4], myCompileTimeKey[7], myCompileTimeKey[10])
#define skCrypt(str) skCrypt_key(str, myCompileTimeKey[4], myCompileTimeKey[7], myCompileTimeKey[10])

#define skCrypt_key(str, key1, key2, key3) []() { \
			constexpr static auto crypted = skc::skCrypter \
				<sizeof(str) / sizeof(str[0]), key1, key2, key3, skc::clean_type<decltype(str[0])>>((skc::clean_type<decltype(str[0])>*)str); \
					return crypted; }()
