// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#ifndef PCH_H
#define PCH_H

// 여기에 미리 컴파일하려는 헤더 추가
#include "framework.h"





// Query의 결과에 해당하는 데이터를 User 객체와 연결하기 위한 코드 구성
void SetRecordInfo(void* ap_owner, HSTMT ah_statement, void* ap_data);

// SQL 명령문에 의해서 가져온 데이터들을 자신이 원하는 형태로 변환하거나 저장하는 작업
// ap_owner: 윈도우 핸들, ap_data: 읽은 데이터, a_step_index: 0부터 시작해서 데이터를 하나 읽을때마다 1씩 증가
// a_count: 읽은 데이터 개수, ap_state: 읽어온 데이터의 상태, option: 검색 조건
int ResultRecord(void* ap_owner, int a_step_index, void* ap_data, ULONG a_count, unsigned short* ap_state, int option);






#endif //PCH_H
