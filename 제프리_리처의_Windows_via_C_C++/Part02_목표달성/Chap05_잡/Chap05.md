# Chepter 05 잡
# Chapter 05 잡
마이크로소프트 윈도우는 잡 커널 오브젝트를 이용하여 프로세스를 그룹으로 관리할 수 있다.
* 잡 오브젝트를 프로세스의 컨테이너와 같은 역할을 수행한다고보면 편하다.

하나의 프로세스만 담고 있는 잡 오브젝트를 생성할 수도 있다.
* 프로세스의 일부 기능을 제한하는 작업 등을 수행할 수있다.

현재 프로세스가 다른 잡 내에 포함되어 있는지 확인하는 함수

BOOL IsProcessInJob(\
&nbsp;&nbsp;&nbsp;&nbsp;HANDLE hProcess,\
&nbsp;&nbsp;&nbsp;&nbsp;HANDLE hJob,\
&nbsp;&nbsp;&nbsp;&nbsp;PBOOL pbInJob);
* 만일 프로세스가 이미 다른 잡 내에 포함되어 있는 경우 프로세스를 다른 잡으로 옮길 수 없다.
* 잡의 제약사항 내에서 수행되던 프로세스가 임의로 잡으로부터 빠져나오는 것을 막기위한 보안 제약사항 중 하나

CreateJobObject(\
&nbsp;&nbsp;&nbsp;&nbsp;PSECURITY_ATTRIBUTES psa;\
&nbsp;&nbsp;&nbsp;&nbsp;PCTSTR pszName);
* 잡 커널 오브젝트 생성
* pszName에 이름을 지정하면 OpenJobObject 함수를 이용하여 동일한 잡 오브젝트를 사용할 수 있다.

HANDLE OpenJobObject(\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD dwDesiredAccess,\
&nbsp;&nbsp;&nbsp;&nbsp;BOOL bInheritHandle,\
&nbsp;&nbsp;&nbsp;&nbsp;PCTSTR pszName);

더 이상 잡 커널오브젝트에 접근할 필요가 없다면 CloseHandle
* 잡 커널 오브젝트가 파괴되어도 잡 내의 모든 프로세스가 종료되지는 않는다.
* 잡 내에 프로세스가 존재한다면 앞으로 파괴될 것이라는 표시만 한다.

잡 핸들을 제거하면 실제로 잡 오브젝트가 존재한다 하더라도 어떤 프로세스에서도 잡 오브젝트에 접근할 수 없다.


## Section 01 - 잡 내의 프로세스에 대한 제한사항 설정
몇 가지 제한사항 형태
* 기본 제한사항과 확장 제한사항은 잡 내의 프로세스가 시스템 리소스를 독점하지 못하도록 한다.
* 기본 UI 제한사항은 잡 내의 프로세스가 사용자 인터페이스를 사용하지 못하도록 한다.
* 보안 제한사항은 잡 내의 프로세스가 보안 자원에 접근하지 못하도록 한다.

잡에 제한사항을 설정하는 함수

BOOL SetInformationJobObject(\
&nbsp;&nbsp;&nbsp;&nbsp;HANDLE hJob,\
&nbsp;&nbsp;&nbsp;&nbsp;JOBOBJECTINFOCLASS JobObjectInformationClass,\
&nbsp;&nbsp;&nbsp;&nbsp;PVOID pJobObjectInformation,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD cbJobObjectInformationSize);
* hJob : 잡 커널 오브젝트 핸들
* JobObjectInformationClass : 설정하고자하는 제한사항의 형태
* pJobObjectInformation : 제한사항 설정 값을 담고있는 구조체 포인터
* cbJobObjectInformationSize : 구조체의 크기

제한사항 형태 : 두 번째 매개변수 : 세 번째 매개변수의 구조체
* 기본 제한사항 : JobObjectBasicLimitInformation : JOBOBJECT_BASIC_LIMIT_INFORMATION
* 확장 제한사항 : JobObjectExtendedLimitInformation : JOBOBJECT_EXTENDED_LIMIT_INFORMATION

StartRestrictedProcess 함수에서는 잡에 대한 기본 제한사항만을 설정하였다.

JOBOBJECT_BASIC_LIMIT_INFORMATION 구조체는 다음과 같다.
~~~c++
typedef struct _JOBOBJECT_BASIC_LIMIT_INFORMATION
{
    LARGE_INTEGER   PerProcessUserTimeLimit;
    LARGE_INTEGER   PerJobUserTimeLimit;
    DWORD           LimitFlags;
    DWORD           MinimumWorkingSetSize;
    DWORD           MaximumWorkingSetSize;
    DWORD           ActiveProcessLimit;
    DWORD_PTR       Affinity;
    DWORD           PriorityClass;
    DWORD           SchedulingClass;
} JOBOBJECT_BASIC_LIMIT_INFORMATION, *PJOBOBJECT_BASIC_LIMIT_INFORMATION;
~~~

* LimitFlags : 잡에 설정하고자 하는 제한사항을 비트 단위로 설정하는 데 사용
* StartRestrictedProcess 함수에서는 JOB_OBJECT_LIMIT_PRIORITY_CLASS, JOB_OBJECT_LIMIT_JOB_TIME 비트만을 설정하였다.
* CPU 선호도, 워킹셋 크기, 프로세스당 CPU 시간 등은 설정하지 않았다.