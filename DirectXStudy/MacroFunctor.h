#pragma once
#ifndef __MACROFUNCTOR_H__
#define __MACROFUNCTOR_H__

//릴리즈함수를 모아놓은 템플릿 클래스 함수 정의하기
//템플릿 클래스는 헤더파일에 정의해야한다.


#include <memory>
#include <type_traits>

//======================================================================
// 플랫폼별 디버그 브레이크
//======================================================================
#if defined(_MSC_VER)
#include <intrin.h>
#define CORE_DEBUG_BREAK() __debugbreak()
#elif defined(__clang__) || defined(__GNUC__)
#define CORE_DEBUG_BREAK() __builtin_trap()
#else
#include <cstdlib>
#define CORE_DEBUG_BREAK() std::abort()
#endif

namespace MacroFunctor
{
    //======================================================================
    // 1. 해제(Release) 계열 — 완전한 함수 객체 대체
    //======================================================================

    struct FReleasePtr
    {
        template <typename T>
        void operator()(T*& InPtr) const noexcept
        {
            static_assert(!std::is_void_v<T>,
                "void* 는 delete 할 수 없습니다. 실제 타입으로 캐스팅하세요.");
            static_assert(sizeof(T) > 0,
                "불완전 타입(전방 선언)은 delete 할 수 없습니다. 정의를 include 하세요.");
            static_assert(!std::is_array_v<T>,
                "배열은 ReleaseArr 를 사용하세요.");

            delete InPtr;      // delete nullptr 은 no-op → 원본의 if 검사 불필요
            InPtr = nullptr;
        }
    };

    struct FReleaseArr
    {
        template <typename T>
        void operator()(T*& InPtr) const noexcept
        {
            static_assert(!std::is_void_v<T>, "void* 는 delete[] 할 수 없습니다.");
            static_assert(sizeof(T) > 0, "불완전 타입은 delete[] 할 수 없습니다.");

            delete[] InPtr;
            InPtr = nullptr;
        }
    };

    struct FReleaseComPtr
    {
        // 반환값: Release() 직후의 참조 카운트 (누수 추적에 유용, 무시해도 됨)
        template <typename T>
        unsigned long operator()(T*& InPtr) const noexcept
        {
            static_assert(sizeof(T) > 0, "불완전 타입은 Release() 를 호출할 수 없습니다.");

            unsigned long RefCount = 0;
            if (InPtr)
            {
                RefCount = InPtr->Release();
                InPtr = nullptr;
            }
            return RefCount;
        }
    };

    //======================================================================
    // 2. 검사(Check) 계열 — 판정부만 함수 객체화
    //    early-return 은 언어 차원에서 함수로 표현 불가 → 매크로 래퍼 유지
    //======================================================================

    struct FNullCheck
    {
        // 생포인터, 스마트포인터, HANDLE 등 bool 문맥 변환 가능한 모든 타입
        template <typename T>
        [[nodiscard]] bool operator()(const T& InValue) const noexcept
        {
            if (!InValue)
            {
                CORE_DEBUG_BREAK();
                return false;
            }
            return true;
        }
    };

    struct FHResultCheckHR
    {
        // HRESULT 는 Windows 에서 long 의 typedef.
        // windows.h 의존을 피하기 위해 long 으로 받고, FAILED() 정의(< 0)를 그대로 사용.
        [[nodiscard]] long operator()(long InResult) const noexcept
        {
            if (InResult < 0)
            {
                CORE_DEBUG_BREAK();
                return InResult;
            }
            return S_OK;
        }
    };

    struct FHResultCheckbool
    {
        // HRESULT 는 Windows 에서 long 의 typedef.
        // windows.h 의존을 피하기 위해 long 으로 받고, FAILED() 정의(< 0)를 그대로 사용.
        [[nodiscard]] bool operator()(long InResult) const noexcept
        {
            if (InResult < 0)
            {
                CORE_DEBUG_BREAK();
                return false;
            }
            return true;
        }
    };

    //======================================================================
    // 3. 전역 인스턴스 (C++17 inline variable → ODR 안전)
    //======================================================================

    //inline constexpr FReleasePtr    ReleasePtr{};
   // inline constexpr FReleaseArr    ReleaseArr{};
    //inline constexpr FReleaseComPtr ReleaseComPtr{};
    //inline constexpr FNullCheck     NullCheck{};
    //inline constexpr FHResultCheck  HResultCheck{};

    //======================================================================
    // 4. 보너스 — 스마트 포인터 Deleter (함수 객체화의 실질적 이득)
    //======================================================================

    struct FComDeleter
    {
        template <typename T>
        void operator()(T* InPtr) const noexcept
        {
            if (InPtr) { InPtr->Release(); }
        }
    };

    template <typename T>
    using TComUniquePtr = std::unique_ptr<T, FComDeleter>;
    // 사용 예: TComUniquePtr<ID3D11Buffer> Buffer{ RawBuffer };  → 소멸 시 자동 Release

} // namespace Core




#endif __MACROFUNCTOR_H__