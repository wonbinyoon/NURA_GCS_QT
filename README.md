# NURA GCS 2026

Ground Control Station software for rocket telemetry.

---

## Windows 빌드 가이드

> 아무것도 설치되어 있지 않은 Windows PC 기준으로 작성되었습니다.

---

### 1단계 — 필수 도구 설치

아래 세 가지를 순서대로 설치합니다.

#### 1-1. Visual Studio Build Tools 2022

C++ 컴파일러(MSVC)를 설치합니다.

1. [Visual Studio Build Tools 다운로드](https://visualstudio.microsoft.com/ko/downloads/#build-tools-for-visual-studio-2022) 에서 **Build Tools for Visual Studio 2022** 다운로드
2. 설치 시 **"C++를 사용한 데스크톱 개발"** 워크로드 체크 후 설치

#### 1-2. CMake 3.16 이상

1. [CMake 다운로드](https://cmake.org/download/) 에서 Windows x64 Installer 다운로드
2. 설치 중 **"Add CMake to the system PATH"** 옵션 선택

#### 1-3. Python 3.x

Qt 설치 도구(aqtinstall)를 실행하기 위해 필요합니다.

1. [Python 다운로드](https://www.python.org/downloads/) 에서 최신 Python 3 설치
2. 설치 중 **"Add Python to PATH"** 옵션 반드시 체크

---

### 2단계 — Qt 6.8.3 설치

PowerShell 또는 명령 프롬프트를 열고 아래 명령을 실행합니다.

```powershell
# aqtinstall 설치 (Qt 자동 설치 도구)
pip install aqtinstall

# Qt 6.8.3 설치 (필요한 모듈 포함)
# 설치 경로: C:\Qt  |  소요 시간: 약 2~5분  |  용량: 약 3 GB
python -m aqt install-qt windows desktop 6.8.3 win64_msvc2022_64 `
    --modules qt3d qtcharts qtserialport qtshadertools `
    --outputdir C:\Qt
```

설치가 완료되면 `C:\Qt\6.8.3\msvc2022_64\` 폴더가 생성됩니다.

---

### 3단계 — 소스코드 클론

```powershell
git clone <repository-url>
cd gcs2026v2
```

---

### 4단계 — 빌드

**"Developer Command Prompt for VS 2022"** 를 시작 메뉴에서 찾아서 실행하거나,
일반 PowerShell에서 아래와 같이 vcvars64.bat을 먼저 로드합니다.

#### 방법 A: Developer Command Prompt 사용 (권장)

시작 메뉴 → **"Developer Command Prompt for VS 2022"** 실행 후:

```cmd
cd gcs2026v2

:: CMake 구성
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.8.3/msvc2022_64" -G "NMake Makefiles"

:: 빌드
cmake --build build
```

#### 방법 B: PowerShell 사용

```powershell
cd gcs2026v2

$vsPath = "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

# CMake 구성
cmd /c "call `"$vsPath`" && cmake -S . -B build -DCMAKE_PREFIX_PATH=`"C:/Qt/6.8.3/msvc2022_64`" -G `"NMake Makefiles`""

# 빌드
cmd /c "call `"$vsPath`" && cmake --build build"
```

> **참고**: 첫 빌드 시 GTest를 자동 다운로드하므로 인터넷 연결이 필요합니다.

---

### 5단계 — Qt DLL 배포

빌드 후 최초 1회만 실행합니다. 실행 파일 옆에 필요한 Qt DLL을 자동으로 복사해줍니다.

```cmd
set PATH=C:\Qt\6.8.3\msvc2022_64\bin;%PATH%
windeployqt6.exe build\app\app_main.exe
```

---

### 6단계 — 실행

```cmd
build\app\app_main.exe
```

또는 탐색기에서 `build\app\app_main.exe` 를 더블클릭합니다.

---

## 디렉토리 구조

```
gcs2026v2/
├── app/          # 진입점 (main.cpp)
├── ui/           # Qt 위젯 (MainWindow, PlotPanel, View3D 등)
├── model/        # 데이터 모델 (DataFrame)
├── parser/       # 텔레메트리 파서
├── logger/       # 데이터 로거
├── replay/       # 재생 스토리지
├── simulator/    # 시뮬레이터
├── tests/        # GTest 단위 테스트
└── CMakeLists.txt
```

---

## 테스트 실행

```cmd
set PATH=C:\Qt\6.8.3\msvc2022_64\bin;%PATH%
cd build
ctest --output-on-failure
```

---

## 플랫폼 지원

| 플랫폼 | 상태 |
|--------|------|
| Windows (MSVC 2022) | ✅ 지원 |
| Linux (GCC/Clang) | ✅ 지원 |

> `replay/ReplayStorage` 는 Windows에서 `CreateFileMapping`, Linux에서 `mmap` 을 사용하도록 분기 처리되어 있습니다.

---

## 트러블슈팅

### `Unable to find renderer plugin for rhi` 오류

Qt ShaderTools 모듈이 없거나 windeployqt를 실행하지 않은 경우 발생합니다.

```powershell
# qtshadertools 재설치
python -m aqt install-qt windows desktop 6.8.3 win64_msvc2022_64 --modules qtshadertools --outputdir C:\Qt

# windeployqt 재실행
set PATH=C:\Qt\6.8.3\msvc2022_64\bin;%PATH%
windeployqt6.exe build\app\app_main.exe
```

### `cmake` 명령을 찾을 수 없음

CMake 설치 시 PATH 추가 옵션을 선택하지 않은 경우입니다. 시스템 환경변수 PATH에 CMake 설치 경로(기본값: `C:\Program Files\CMake\bin`)를 수동으로 추가하세요.

### 빌드 시 `cl.exe`를 찾을 수 없음

일반 PowerShell이나 CMD에서 실행한 경우입니다. **Developer Command Prompt for VS 2022** 를 사용하거나 `vcvars64.bat`을 먼저 실행하세요.
