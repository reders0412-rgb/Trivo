# 🔧 Fix: LNK2019 IconProvider::makeBadgeIcon

## 에러

```
IconProvider.obj : error LNK2019: unresolved external symbol
"private: static class QIcon __cdecl IconProvider::makeBadgeIcon(...)"
```

## 원인

`IconProvider.h`에서 `makeBadgeIcon`을 **private static 멤버 함수**로 선언했지만,  
`IconProvider.cpp`에서는 **free function** (일반 함수)으로 정의했기 때문에  
링커가 `IconProvider::makeBadgeIcon`을 찾지 못해 발생한 오류.

```cpp
// ❌ 기존 (cpp) — free function으로 정의됨
static QIcon makeBadgeIcon(const QString &label, unsigned int bgColor) { ... }

// ✅ 수정 — 클래스 멤버 함수로 정의
QIcon IconProvider::makeBadgeIcon(const QString &label, unsigned int bgColor) { ... }
```

## 수정된 파일

- `src/utils/IconProvider.cpp` — line 58: `static QIcon makeBadgeIcon` → `QIcon IconProvider::makeBadgeIcon`

## 참고

헤더의 `private:` 선언은 그대로 유지해도 됨.  
내부에서만 호출되는 helper이므로 `private`이 맞는 설계.
