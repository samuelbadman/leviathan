#include "Core/Platform/UUID.h"

void Core::Uuid::Generate()
{
	UUID Temp;
	const RPC_STATUS Result = UuidCreate(&Temp);

	if (Result == RPC_S_OK || Result == RPC_S_UUID_LOCAL_ONLY)
	{
		Data1 = Temp.Data1;
		Data2 = Temp.Data2;
		Data3 = Temp.Data3;
		Data4[0] = Temp.Data4[0];
		Data4[1] = Temp.Data4[1];
		Data4[2] = Temp.Data4[2];
		Data4[3] = Temp.Data4[3];
		Data4[4] = Temp.Data4[4];
		Data4[5] = Temp.Data4[5];
		Data4[6] = Temp.Data4[6];
		Data4[7] = Temp.Data4[7];
	}
}

bool Core::Uuid::operator==(const Core::Uuid& Rhs)
{
	return (Data1 == Rhs.Data1) &&
		(Data2 == Rhs.Data2) &&
		(Data3 == Rhs.Data3) &&
		(Data4 == Rhs.Data4);
}
