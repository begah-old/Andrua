/*
 * Util_String.c
 *
 *  Created on: Apr 19, 2015
 *      Author: begah
 */

#include "Util.h"

void String_Add(char *String1, char *String2)
{
	int Index1 = 0, I = 0;
	while (1)
	{
		if (String1[Index1] == '\0')
		{
			break;
		}
		Index1++;
	}
	while (1)
	{
		String1[Index1] = String2[I];
		if (String2[I] == '\0')
		{
			break;
		}
		I++;
		Index1++;
	}
}

void String_Copy(char *String1, const char *String2)
{
	int i = 0;
	while (1)
	{
		String1[i] = String2[i];
		if (String2[i] == '\0')
		{
			return;
		}
		i++;
	}
	return;
}

void String_Remove(char *String1, char *String2)
{
	int Length2 = 0, Index1 = 0, InTemp = 0, At = 0;
	while (1)
	{
		if (String2[Length2] == '\0')
		{
			break;
		}
		Length2++;
	}

	while (1)
	{
		if (String1[Index1] == String2[InTemp])
		{
			InTemp++;
			if (InTemp == 1)
			{
				At = Index1;
			}
			else if (InTemp == Length2)
			{
				break;
			}
		}
		else
		{
			InTemp = 0;
			if (String1[Index1] == '\0')
			{
				return;
			}
		}
		Index1++;
	}

	InTemp = 0;
	while (InTemp < Length2)
	{
		String1[At + InTemp] = ' ';
		InTemp++;
	}
	String1[At] = '\0';
}

int String_fulllength(char *String)
{
	int Num = 0;
	while (*String)
	{
		Num++;
		if (*String == '\t')
			Num++;
		String++;
	}
	return Num;
}

int String_numoftab(char *String)
{
	int Num = 0;
	while (*String)
	{
		if (*String == '\t')
			Num++;
		String++;
	}
	return Num;
}

int String_numoftab_Length(const char *String, int Length)
{
	int Num = 0;
	for(int i = 0; i < Length; i++)
	{
		if (String[i] == '\t')
			Num++;
	}
	return Num;
}

int String_toInt(char *String)
{
	int Num = 0;
	if (*String == 0 || *String == '\0')
		return -1;
	while (*String || *String != 0)
	{
		if (*String < '0' || *String > '9')
			return -1;
		Num *= 10;
		Num += *String - '0';
		String++;
	}
	return Num;
}

struct String_Struct *String_Split(char *String, char Spliter, int *NumOfSplit)
{
	struct String_Struct *Struct;
	if (!NumOfSplit)
		NumOfSplit = malloc(sizeof(int));
	*NumOfSplit = 1;
	for (int i = 0;; i++)
	{
		if (String[i] == '\0')
			break;
		if (String[i] == Spliter && String[i + 1] != '\0')
			(*NumOfSplit)++;
	}
	Struct = malloc(sizeof(struct String_Struct) * *NumOfSplit);
	char *StringCopy = String;
	for (int i = 0; i < *NumOfSplit; i++)
	{
		int StringLength = 0;
		char *TempString = StringCopy;
		while (TempString[StringLength] != '\0'
				&& TempString[StringLength] != Spliter)
			StringLength++;
		Struct[i].String = malloc(sizeof(char) * StringLength + 1);
		Struct[i].String = memcpy(Struct[i].String, StringCopy,
				sizeof(char) * StringLength);
		Struct[i].String[StringLength] = '\0';
		StringCopy += StringLength + 1;
	}
	return Struct;
}

_Bool String_contain(const char *ori, const char *Con)
{
	if (*Con == '\0')
		return true;
	if (*ori != *Con || *ori == '\0')
		return false;
	ori++;
	Con++;
	return String_contain(ori, Con);
}

//TODO: Optimize
int String_replace(char *Ligne, const char *OldName, const char *NewName)
{
	char *Ori = Ligne;
	while (!String_contain(Ligne, OldName))
	{
		if (!*Ligne)
			return -1;
		Ligne++;
	}

	int OldName_Length = String_length(OldName);
	int Length_ToKeep = String_length(Ligne + OldName_Length);
	int NewName_Length = String_length(NewName);
	char *Data_ToKeep = malloc(sizeof(char) * Length_ToKeep);
	Data_ToKeep = memcpy(Data_ToKeep, Ligne + OldName_Length,
			sizeof(char) * Length_ToKeep);

	for (int i = 0; i < OldName_Length + Length_ToKeep; i++)
		Ligne[i] = '\0';
	Ligne = memcpy(Ligne, NewName, sizeof(char) * NewName_Length);
	Ligne += NewName_Length;
	Ligne = memcpy(Ligne, Data_ToKeep, sizeof(char) * Length_ToKeep);

	return (Ligne - Ori);
}

_Bool Character_isValid(char C)
{
	if(C < 32 && C != '\0' && C != '	' && C != '\n') // 9 is tab
		return false;
	return true;
}
