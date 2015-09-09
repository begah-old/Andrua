/*
 * Code_Manager.c
 *
 *  Created on: Jun 30, 2015
 *      Author: begah
 */

#include "Code_Manager.h"

struct Lua_Code *Lua_Code_Init(char *Code)
{
	struct Lua_Code *LC = malloc(sizeof(struct Lua_Code));
	LC->Code = vector_new(sizeof(struct Lua_Code_Ligne));

	int NumOfLines = 0;
	struct String_Struct *SS = String_Split(Code, '\n', &NumOfLines);

	for (int i = 0; i < NumOfLines; i++)
	{
		struct Lua_Code_Ligne *SS2 = malloc(sizeof(struct Lua_Code_Ligne));
		SS2->Ligne = malloc(sizeof(char) * (CODE_MAX_LENGTH + 1));
		for (int i = 0; i <= CODE_MAX_LENGTH; i++)
			SS2->Ligne[i] = '\0';
		SS2->Ligne = memcpy(SS2->Ligne, SS[i].String,
				sizeof(char) * String_length(SS[i].String));
		SS2->Static = false;
		vector_push_back(LC->Code, SS2);
		free(SS2);
	}
	free(SS);

	LC->Static_Variable = vector_new(sizeof(struct Int_Pointer));
	return LC;
}

struct Lua_Code *Lua_Code_OpenFile(char *Path)
{
	struct F_FileExternal *f = FileExternal_Open(Path);
	int Length = FileExternal_Length(f);
	char *Data = malloc(sizeof(char) * (Length + 1));
	FileExternal_Read(Data, sizeof(char), Length, f);
	Data[Length] = '\0';

	for(int i = 0; i < Length; i++)
		Data[i] = Character_isValid(Data[i]) ? Data[i] : ' ';

	struct Lua_Code *Code = Lua_Code_Init(Data);
	free(Data);

	FileExternal_Close(f);

	return Code;
}

struct Lua_Code *Lua_Code_Copy(struct Lua_Code *LC)
{
	struct Lua_Code *LC2 = malloc(sizeof(struct Lua_Code));
	LC2->Code = vector_new(sizeof(struct Lua_Code_Ligne));

	struct Lua_Code_Ligne *Lignes = (struct Lua_Code_Ligne *) LC->Code->items;
	struct Lua_Code_Ligne *LCL = malloc(sizeof(struct Lua_Code_Ligne));

	for (int i = 0; i < LC->Code->size; i++)
	{
		LCL->Ligne = malloc(sizeof(char) * (CODE_MAX_LENGTH + 1));
		LCL->Ligne = memcpy(LCL->Ligne, Lignes[i].Ligne,
				sizeof(char) * (CODE_MAX_LENGTH + 1));
		LCL->Static = Lignes[i].Static;
		vector_push_back(LC2->Code, LCL);
	}

	free(LCL);

	LC2->Static_Variable = vector_new(sizeof(struct Int_Pointer));
	struct Int_Pointer *IPointers =
			(struct Int_Pointer *) LC->Static_Variable->items;

	struct Int_Pointer *IP = malloc(sizeof(struct Int_Pointer));
	for (int i = 0; i < LC->Static_Variable->size; i++)
	{
		IP->Integer = IPointers[i].Integer;
		vector_push_back(LC2->Static_Variable, IP);
	}
	free(IP);
	return LC2;
}

void Lua_Code_Copy_noMalloc(struct Lua_Code *LC, struct Lua_Code *LC2)
{
	vector_delete(LC2->Code);
	vector_delete(LC2->Static_Variable);

	LC2->Code = vector_new(sizeof(struct Lua_Code_Ligne));

	struct Lua_Code_Ligne *Lignes = (struct Lua_Code_Ligne *) LC->Code->items;

	for (int i = 0; i < LC->Code->size; i++)
	{
		struct Lua_Code_Ligne LCL;
		LCL.Ligne = malloc(sizeof(char) * (CODE_MAX_LENGTH + 1));
		LCL.Ligne = memcpy(LCL.Ligne, Lignes[i].Ligne,
				sizeof(char) * (CODE_MAX_LENGTH + 1));
		LCL.Static = Lignes[i].Static;
		vector_push_back(LC2->Code, &LCL);
	}

	LC2->Static_Variable = vector_new(sizeof(struct Int_Pointer));
	struct Int_Pointer *IPointers =
			(struct Int_Pointer *) LC->Static_Variable->items;

	struct Int_Pointer *IP = malloc(sizeof(struct Int_Pointer));
	for (int i = 0; i < LC->Static_Variable->size; i++)
	{
		IP->Integer = IPointers[i].Integer;
		vector_push_back(LC2->Static_Variable, IP);
	}
	free(IP);
}

void Lua_Code_addLigne(struct Lua_Code *LC, char *Ligne, _Bool Static)
{
	struct Lua_Code_Ligne *LCL = malloc(sizeof(struct Lua_Code_Ligne));
	LCL->Ligne = malloc(sizeof(char) * (CODE_MAX_LENGTH + 1));
	for (int I = 0; I < CODE_MAX_LENGTH + 1; I++)
		LCL->Ligne[I] = '\0';
	LCL->Ligne = memcpy(LCL->Ligne, Ligne, sizeof(char) * String_length(Ligne));
	LCL->Static = Static;
	vector_push_back(LC->Code, LCL);
	free(LCL);
}

int *Lua_Code_addVariable(struct Lua_Code *LC, const char *Variables)
{
	struct Int_Pointer *IP = malloc(sizeof(struct Int_Pointer));
	IP->Integer = malloc(sizeof(int));

	*IP->Integer = LC->Static_Variable->size;
	struct Lua_Code_Ligne *LCL = malloc(sizeof(struct Lua_Code_Ligne));
	LCL->Ligne = malloc(sizeof(char) * (CODE_MAX_LENGTH + 1));
	for (int I = 0; I < CODE_MAX_LENGTH + 1; I++)
		LCL->Ligne[I] = '\0';
	LCL->Ligne = memcpy(LCL->Ligne, Variables,
			sizeof(char) * String_length(Variables));
	LCL->Static = true;
	vector_insert(LC->Code, *IP->Integer, LCL);
	free(LCL);
	vector_push_back(LC->Static_Variable, IP);
	free(IP);
	return ((struct Int_Pointer *) LC->Static_Variable->items
			+ LC->Static_Variable->size - 1)->Integer;
}

void Lua_Code_changeVariable(struct Lua_Code *LC, int *ID, const char *OldName,
		const char *NewName)
{
	struct Lua_Code_Ligne *LCL = (struct Lua_Code_Ligne *) LC->Code->items
			+ *ID;
	char * Ligne = LCL->Ligne;
	while (1)
	{
		int i = String_replace(Ligne, OldName, NewName);
		if (i < 0)
			break;
		Ligne += i;
	}
}

void Lua_Code_removeVariable(struct Lua_Code *LC, int *ID)
{
	if (!ID)
		return;

	free(((struct Lua_Code_Ligne *) LC->Code->items + *ID)->Ligne);
	vector_erase(LC->Code, *ID);
	vector_erase(LC->Static_Variable, *ID);

	for (int i = *ID; i < LC->Static_Variable->size; i++)
		(*((struct Int_Pointer *) LC->Static_Variable->items + i)->Integer)--;

	free(ID);
}

void Lua_Code_clear(struct Lua_Code *LC)
{
	for (int i = LC->Static_Variable->size - 1; i > 0; i++)
	{
		Lua_Code_removeVariable(LC, (int *) LC->Static_Variable->items + i);
	}

	for (int i = 0; i < LC->Code->size; i++)
	{
		free(((struct Lua_Code_Ligne *) LC->Code->items)[i].Ligne);
	}

	vector_delete(LC->Code);
	LC->Code = vector_new(sizeof(struct Lua_Code_Ligne));
}

char *Lua_Code_toString(struct Lua_Code *LC)
{
	int Lengths[LC->Code->size], FullLength = 0;

	struct Lua_Code_Ligne *LCL = LC->Code->items;

	for(int i = 0; i < LC->Code->size; i++)
	{
		Lengths[i] = String_length(LCL[i].Ligne) + 1;
		FullLength += Lengths[i];
	}

	char *String = malloc(sizeof(char) * FullLength);

	int At = 0;

	for(int i = 0; i < LC->Code->size; i++)
	{
		memcpy(String + At, LCL[i].Ligne, sizeof(char) * (Lengths[i] - 1));
		String[At + Lengths[i] - 1] = '\n';
		At += Lengths[i];
	}

	String[At - 1] = '\0';

	return String;
}

void Lua_Code_Free(struct Lua_Code **Code)
{
	struct Lua_Code_Ligne *SS = (*Code)->Code->items;

	for (int LigneNum = 0; LigneNum < (*Code)->Code->size; LigneNum++)
	{
		free(SS[LigneNum].Ligne);
	}
	vector_delete((*Code)->Code);

	free((*Code));
	(*Code) = NULL;
}
