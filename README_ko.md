
# cPlayer - Console-based MP3 Music Player

**cPlayer**는 C++로 개발된 콘솔 기반 MP3 음악 플레이어입니다.  
간단한 UI와 기본적인 재생 기능을 제공하며, 파일 또는 폴더의 MP3 파일을 로드하여 재생할 수 있습니다.

---

## 주요 기능

- **MP3 파일 재생**: 단일 파일 또는 폴더에서 MP3 파일을 로드하여 재생 가능.
- **기본 재생 제어**:
  - 재생/일시정지, 다음 곡, 이전 곡.
  - 셔플 모드 및 반복 모드 지원.
- **사용자 입력**:
  - 마우스 입력을 통한 진행 바 클릭 및 곡 선택.
  - 키보드 단축키로 빠르게 제어 가능.
- **파일 관리**:
  - MP3 파일의 ID3 태그 제거.
  - 임시 파일 생성 및 숨김 처리.

---

## 사용법

1. **파일 실행**  
   빌드된 실행 파일을 실행합니다.

2. **파일/폴더 추가**  
   - `o` 키: 파일 열기 대화상자를 통해 MP3 파일 추가.
   - `p` 키: 폴더 열기 대화상자를 통해 폴더 전체의 MP3 파일 추가.

3. **재생 제어**  
   - 스페이스바: 재생/일시정지.
   - `,` 키: 이전 곡 재생.
   - `.` 키: 다음 곡 재생.
   - `s` 키: 셔플 모드 전환.
   - `r` 키: 반복 모드 전환.

4. **종료**  
   프로그램 종료 시 임시 파일이 자동으로 삭제됩니다.

---

## 개발 환경

- **언어**: C++
- **플랫폼**: Windows
- **빌드 도구**: CLion, MinGW, Visual Studio 지원
- **필요 라이브러리**:
  - `winmm.lib`: Windows Multimedia API
  - `windows.h`, `mmsystem.h` 등 Windows API 헤더

---

## 프로젝트 구조

```
cPlayer/
│
├── src/
│   ├── main.cpp            # 프로그램의 진입점
│   ├── musicPlayer.cpp     # 음악 재생 관리 (MCI 사용)
│   ├── fileManager.cpp     # 파일 및 폴더 관리
│   ├── playerUI.cpp        # 콘솔 UI 처리
│   ├── manageConsole.cpp   # 콘솔 창 및 입력 관리
│   ├── dynamicArray.cpp    # 동적 배열 처리
│
├── include/
│   ├── musicPlayer.h       # 음악 재생 관련 헤더
│   ├── fileManager.h       # 파일 관리 관련 헤더
│   ├── playerUI.h          # UI 관리 관련 헤더
│   ├── manageConsole.h     # 콘솔 관리 관련 헤더
│   ├── dynamicArray.h      # 동적 배열 관련 헤더
│
└── README.md               # 프로젝트 설명 파일
```

---

## 주요 단축키

| 입력 키      | 동작                   |
|--------------|------------------------|
| `o`          | 파일 열기              |
| `p`          | 폴더 열기              |
| `스페이스바` | 재생/일시정지          |
| `,`          | 이전 곡                |
| `.`          | 다음 곡                |
| `s`          | 셔플 모드 전환         |
| `r`          | 반복 모드 전환         |

---

## 기여하기

이 프로젝트는 오픈소스입니다. 개선 사항, 버그 제보 또는 새로운 기능 추가를 원하신다면 [Issues](https://github.com/USERNAME/cPlayer/issues) 또는 [Pull Requests](https://github.com/USERNAME/cPlayer/pulls)를 통해 기여해주세요.

---

## 라이선스

MIT License. 자세한 내용은 [LICENSE](LICENSE) 파일을 참조하세요.
