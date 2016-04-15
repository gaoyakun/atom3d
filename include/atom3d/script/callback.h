#ifndef __ATOM_SCRIPT_CALLBACK_H
#define __ATOM_SCRIPT_CALLBACK_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_kernel.h"
#include "script.h"
#include "var.h"

struct ATOM_SCRIPT_API ATOM_ScriptCallbackBridge
{
  static void SetCallback (ATOM_Script *scp, const char *func);
  static void GetCallback (ATOM_Script **scp, const char **func);
  static ATOM_Mutex lock;
  static ATOM_Script *script;
  static const char *function;
};

template <class T> struct ATOM_ScriptCallbackT;

template <>
struct ATOM_ScriptCallbackT<void (*)(void)>
{
  static void execute (void)
  {
    ATOM_Script *scp = 0;
    const char *func = 0;
    ATOM_ScriptCallbackBridge::GetCallback (&scp, &func);
    scp->setFunction (func);
    scp->setNumParameters (0);
    scp->setNumResults (0);
    scp->execute ();
  }
};

template <class T1>
struct ATOM_ScriptCallbackT<void (*)(T1)>
{
  static void execute (T1 param1)
  {
    ATOM_Script *scp = 0;
    const char *func = 0;
    ATOM_ScriptCallbackBridge::GetCallback (&scp, &func);
    scp->setFunction (func);
    scp->setNumParameters (1);
    scp->setParameter (0, param1);
    scp->setNumResults (0);
    scp->execute ();
  }
};

template <class T1, class T2>
struct ATOM_ScriptCallbackT<void (*)(T1, T2)>
{
  static void execute (T1 param1, T2 param2)
  {
    ATOM_Script *scp = 0;
    const char *func = 0;
    ATOM_ScriptCallbackBridge::GetCallback (&scp, &func);
    scp->setFunction (func);
    scp->setNumParameters (2);
    scp->setParameter (0, param1);
    scp->setParameter (1, param2);
    scp->setNumResults (0);
    scp->execute ();
  }
};

template <class T1, class T2, class T3>
struct ATOM_ScriptCallbackT<void (*)(T1, T2, T3)>
{
  static void execute (T1 param1, T2 param2, T3 param3)
  {
    ATOM_Script *scp = 0;
    const char *func = 0;
    ATOM_ScriptCallbackBridge::GetCallback (&scp, &func);
    scp->setFunction (func);
    scp->setNumParameters (3);
    scp->setParameter (0, param1);
    scp->setParameter (1, param2);
    scp->setParameter (2, param3);
    scp->setNumResults (0);
    scp->execute ();
  }
};

template <class T1, class T2, class T3, class T4>
struct ATOM_ScriptCallbackT<void (*)(T1, T2, T3, T4)>
{
  static void execute (T1 param1, T2 param2, T3 param3, T4 param4)
  {
    ATOM_Script *scp = 0;
    const char *func = 0;
    ATOM_ScriptCallbackBridge::GetCallback (&scp, &func);
    scp->setFunction (func);
    scp->setNumParameters (4);
    scp->setParameter (0, param1);
    scp->setParameter (1, param2);
    scp->setParameter (2, param3);
    scp->setParameter (3, param4);
    scp->setNumResults (0);
    scp->execute ();
  }
};

template <class T1, class T2, class T3, class T4, class T5>
struct ATOM_ScriptCallbackT<void (*)(T1, T2, T3, T4, T5)>
{
  static void execute (T1 param1, T2 param2, T3 param3, T4 param4, T5 param5)
  {
    ATOM_Script *scp = 0;
    const char *func = 0;
    ATOM_ScriptCallbackBridge::GetCallback (&scp, &func);
    scp->setFunction (func);
    scp->setNumParameters (5);
    scp->setParameter (0, param1);
    scp->setParameter (1, param2);
    scp->setParameter (2, param3);
    scp->setParameter (3, param4);
    scp->setParameter (4, param5);
    scp->setNumResults (0);
    scp->execute ();
  }
};

template <class T1, class T2, class T3, class T4, class T5, class T6>
struct ATOM_ScriptCallbackT<void (*)(T1, T2, T3, T4, T5, T6)>
{
  static void execute (T1 param1, T2 param2, T3 param3, T4 param4, T5 param5, T6 param6)
  {
    ATOM_Script *scp = 0;
    const char *func = 0;
    ATOM_ScriptCallbackBridge::GetCallback (&scp, &func);
    scp->setFunction (func);
    scp->setNumParameters (6);
    scp->setParameter (0, param1);
    scp->setParameter (1, param2);
    scp->setParameter (2, param3);
    scp->setParameter (3, param4);
    scp->setParameter (4, param5);
    scp->setParameter (5, param6);
    scp->setNumResults (0);
    scp->execute ();
  }
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
struct ATOM_ScriptCallbackT<void (*)(T1, T2, T3, T4, T5, T6, T7)>
{
  static void execute (T1 param1, T2 param2, T3 param3, T4 param4, T5 param5, T6 param6, T7 param7)
  {
    ATOM_Script *scp = 0;
    const char *func = 0;
    ATOM_ScriptCallbackBridge::GetCallback (&scp, &func);
    scp->setFunction (func);
    scp->setNumParameters (7);
    scp->setParameter (0, param1);
    scp->setParameter (1, param2);
    scp->setParameter (2, param3);
    scp->setParameter (3, param4);
    scp->setParameter (4, param5);
    scp->setParameter (5, param6);
    scp->setParameter (6, param7);
    scp->setNumResults (0);
    scp->execute ();
  }
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
struct ATOM_ScriptCallbackT<void (*)(T1, T2, T3, T4, T5, T6, T7, T8)>
{
  static void execute (T1 param1, T2 param2, T3 param3, T4 param4, T5 param5, T6 param6, T7 param7, T8 param8)
  {
    ATOM_Script *scp = 0;
    const char *func = 0;
    ATOM_ScriptCallbackBridge::GetCallback (&scp, &func);
    scp->setFunction (func);
    scp->setNumParameters (8);
    scp->setParameter (0, param1);
    scp->setParameter (1, param2);
    scp->setParameter (2, param3);
    scp->setParameter (3, param4);
    scp->setParameter (4, param5);
    scp->setParameter (5, param6);
    scp->setParameter (6, param7);
    scp->setParameter (7, param8);
    scp->setNumResults (0);
    scp->execute ();
  }
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct ATOM_ScriptCallbackT<void (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9)>
{
  static void execute (T1 param1, T2 param2, T3 param3, T4 param4, T5 param5, T6 param6, T7 param7, T8 param8, T9 param9)
  {
    ATOM_Script *scp = 0;
    const char *func = 0;
    ATOM_ScriptCallbackBridge::GetCallback (&scp, &func);
    scp->setFunction (func);
    scp->setNumParameters (9);
    scp->setParameter (0, param1);
    scp->setParameter (1, param2);
    scp->setParameter (2, param3);
    scp->setParameter (3, param4);
    scp->setParameter (4, param5);
    scp->setParameter (5, param6);
    scp->setParameter (6, param7);
    scp->setParameter (7, param8);
    scp->setParameter (8, param9);
    scp->setNumResults (0);
    scp->execute ();
  }
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
struct ATOM_ScriptCallbackT<void (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10)>
{
  static void execute (T1 param1, T2 param2, T3 param3, T4 param4, T5 param5, T6 param6, T7 param7, T8 param8, T9 param9, T10 param10)
  {
    ATOM_Script *scp = 0;
    const char *func = 0;
    ATOM_ScriptCallbackBridge::GetCallback (&scp, &func);
    scp->setFunction (func);
    scp->setNumParameters (10);
    scp->setParameter (0, param1);
    scp->setParameter (1, param2);
    scp->setParameter (2, param3);
    scp->setParameter (3, param4);
    scp->setParameter (4, param5);
    scp->setParameter (5, param6);
    scp->setParameter (6, param7);
    scp->setParameter (7, param8);
    scp->setParameter (8, param9);
    scp->setParameter (9, param10);
    scp->setNumResults (0);
    scp->execute ();
  }
};

template <class Tr>
struct ATOM_ScriptCallbackT<Tr (*)(void)>
{
  static Tr execute (void)
  {
    ATOM_Script *scp = 0;
    const char *func = 0;
    ATOM_ScriptCallbackBridge::GetCallback (&scp, &func);
    scp->setFunction (func);
    scp->setNumParameters (0);
    scp->setNumResults (1);
    scp->execute ();
    return ATOM_ScriptVariantCast<Tr>()(scp->getResult(0));
  }
};

template <class Tr, class T1>
struct ATOM_ScriptCallbackT<Tr (*)(T1)>
{
  static Tr execute (T1 param1)
  {
    ATOM_Script *scp = 0;
    const char *func = 0;
    ATOM_ScriptCallbackBridge::GetCallback (&scp, &func);
    scp->setFunction (func);
    scp->setNumParameters (1);
    scp->setParameter (0, param1);
    scp->setNumResults (1);
    scp->execute ();
    return ATOM_ScriptVariantCast<Tr>()(scp->getResult(0));
  }
};

template <class Tr, class T1, class T2>
struct ATOM_ScriptCallbackT<Tr (*)(T1, T2)>
{
  static Tr execute (T1 param1, T2 param2)
  {
    ATOM_Script *scp = 0;
    const char *func = 0;
    ATOM_ScriptCallbackBridge::GetCallback (&scp, &func);
    scp->setFunction (func);
    scp->setNumParameters (2);
    scp->setParameter (0, param1);
    scp->setParameter (1, param2);
    scp->setNumResults (1);
    scp->execute ();
    return ATOM_ScriptVariantCast<Tr>()(scp->getResult(0));
  }
};

template <class Tr, class T1, class T2, class T3>
struct ATOM_ScriptCallbackT<Tr (*)(T1, T2, T3)>
{
  static Tr execute (T1 param1, T2 param2, T3 param3)
  {
    ATOM_Script *scp = 0;
    const char *func = 0;
    ATOM_ScriptCallbackBridge::GetCallback (&scp, &func);
    scp->setFunction (func);
    scp->setNumParameters (3);
    scp->setParameter (0, param1);
    scp->setParameter (1, param2);
    scp->setParameter (2, param3);
    scp->setNumResults (1);
    scp->execute ();
    return ATOM_ScriptVariantCast<Tr>()(scp->getResult(0));
  }
};

template <class Tr, class T1, class T2, class T3, class T4>
struct ATOM_ScriptCallbackT<Tr (*)(T1, T2, T3, T4)>
{
  static Tr execute (T1 param1, T2 param2, T3 param3, T4 param4)
  {
    ATOM_Script *scp = 0;
    const char *func = 0;
    ATOM_ScriptCallbackBridge::GetCallback (&scp, &func);
    scp->setFunction (func);
    scp->setNumParameters (4);
    scp->setParameter (0, param1);
    scp->setParameter (1, param2);
    scp->setParameter (2, param3);
    scp->setParameter (3, param4);
    scp->setNumResults (1);
    scp->execute ();
    return ATOM_ScriptVariantCast<Tr>()(scp->getResult(0));
  }
};

template <class Tr, class T1, class T2, class T3, class T4, class T5>
struct ATOM_ScriptCallbackT<Tr (*)(T1, T2, T3, T4, T5)>
{
  static Tr execute (T1 param1, T2 param2, T3 param3, T4 param4, T5 param5)
  {
    ATOM_Script *scp = 0;
    const char *func = 0;
    ATOM_ScriptCallbackBridge::GetCallback (&scp, &func);
    scp->setFunction (func);
    scp->setNumParameters (5);
    scp->setParameter (0, param1);
    scp->setParameter (1, param2);
    scp->setParameter (2, param3);
    scp->setParameter (3, param4);
    scp->setParameter (4, param5);
    scp->setNumResults (1);
    scp->execute ();
    return ATOM_ScriptVariantCast<Tr>()(scp->getResult(0));
  }
};

template <class Tr, class T1, class T2, class T3, class T4, class T5, class T6>
struct ATOM_ScriptCallbackT<Tr (*)(T1, T2, T3, T4, T5, T6)>
{
  static Tr execute (T1 param1, T2 param2, T3 param3, T4 param4, T5 param5, T6 param6)
  {
    ATOM_Script *scp = 0;
    const char *func = 0;
    ATOM_ScriptCallbackBridge::GetCallback (&scp, &func);
    scp->setFunction (func);
    scp->setNumParameters (6);
    scp->setParameter (0, param1);
    scp->setParameter (1, param2);
    scp->setParameter (2, param3);
    scp->setParameter (3, param4);
    scp->setParameter (4, param5);
    scp->setParameter (5, param6);
    scp->setNumResults (1);
    scp->execute ();
    return ATOM_ScriptVariantCast<Tr>()(scp->getResult(0));
  }
};

template <class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
struct ATOM_ScriptCallbackT<Tr (*)(T1, T2, T3, T4, T5, T6, T7)>
{
  static Tr execute (T1 param1, T2 param2, T3 param3, T4 param4, T5 param5, T6 param6, T7 param7)
  {
    ATOM_Script *scp = 0;
    const char *func = 0;
    ATOM_ScriptCallbackBridge::GetCallback (&scp, &func);
    scp->setFunction (func);
    scp->setNumParameters (7);
    scp->setParameter (0, param1);
    scp->setParameter (1, param2);
    scp->setParameter (2, param3);
    scp->setParameter (3, param4);
    scp->setParameter (4, param5);
    scp->setParameter (5, param6);
    scp->setParameter (6, param7);
    scp->setNumResults (1);
    scp->execute ();
    return ATOM_ScriptVariantCast<Tr>()(scp->getResult(0));
  }
};

template <class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
struct ATOM_ScriptCallbackT<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8)>
{
  static Tr execute (T1 param1, T2 param2, T3 param3, T4 param4, T5 param5, T6 param6, T7 param7, T8 param8)
  {
    ATOM_Script *scp = 0;
    const char *func = 0;
    ATOM_ScriptCallbackBridge::GetCallback (&scp, &func);
    scp->setFunction (func);
    scp->setNumParameters (8);
    scp->setParameter (0, param1);
    scp->setParameter (1, param2);
    scp->setParameter (2, param3);
    scp->setParameter (3, param4);
    scp->setParameter (4, param5);
    scp->setParameter (5, param6);
    scp->setParameter (6, param7);
    scp->setParameter (7, param8);
    scp->setNumResults (1);
    scp->execute ();
    return ATOM_ScriptVariantCast<Tr>()(scp->getResult(0));
  }
};

template <class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct ATOM_ScriptCallbackT<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9)>
{
  static Tr execute (T1 param1, T2 param2, T3 param3, T4 param4, T5 param5, T6 param6, T7 param7, T8 param8, T9 param9)
  {
    ATOM_Script *scp = 0;
    const char *func = 0;
    ATOM_ScriptCallbackBridge::GetCallback (&scp, &func);
    scp->setFunction (func);
    scp->setNumParameters (9);
    scp->setParameter (0, param1);
    scp->setParameter (1, param2);
    scp->setParameter (2, param3);
    scp->setParameter (3, param4);
    scp->setParameter (4, param5);
    scp->setParameter (5, param6);
    scp->setParameter (6, param7);
    scp->setParameter (7, param8);
    scp->setParameter (8, param9);
    scp->setNumResults (1);
    scp->execute ();
    return ATOM_ScriptVariantCast<Tr>()(scp->getResult(0));
  }
};

template <class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
struct ATOM_ScriptCallbackT<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10)>
{
  static Tr execute (T1 param1, T2 param2, T3 param3, T4 param4, T5 param5, T6 param6, T7 param7, T8 param8, T9 param9, T10 param10)
  {
    ATOM_Script *scp = 0;
    const char *func = 0;
    ATOM_ScriptCallbackBridge::GetCallback (&scp, &func);
    scp->setFunction (func);
    scp->setNumParameters (10);
    scp->setParameter (0, param1);
    scp->setParameter (1, param2);
    scp->setParameter (2, param3);
    scp->setParameter (3, param4);
    scp->setParameter (4, param5);
    scp->setParameter (5, param6);
    scp->setParameter (6, param7);
    scp->setParameter (7, param8);
    scp->setParameter (8, param9);
    scp->setParameter (9, param10);
    scp->setNumResults (1);
    scp->execute ();
    return ATOM_ScriptVariantCast<Tr>()(scp->getResult(0));
  }
};

#pragma pack(push, 1)
struct ATOM_DynamicFunction
{
  unsigned char pushaop0; //0x66
  unsigned char pushaop1; //0x60
  unsigned char pushop1; //0x68
  const void *imm0;
  unsigned char pushop2; //0x68
  const void *imm1;
  unsigned char callop; //0xe8
  int funcoffset;
  unsigned char addesp8_0; //0x83
  unsigned char addesp8_1; //0xc4
  unsigned char addesp8_2; //0x08
  unsigned char popaop0; //0x66
  unsigned char popaop1; //0x61
  unsigned char jmpop;
  int jmpoffset;
  void create (const void *imm0, const void *imm1, void *callfunc, void *dst);
};
#pragma pack(pop)

#endif //__ATOM_SCRIPT_CALLBACK_H

