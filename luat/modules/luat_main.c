

#include "luat_base.h"
#include "luat_malloc.h"
//#include "luat_fs.h"
#include "luat_log.h"
#include "stdio.h"

static lua_State *L;

lua_State * luat_get_state() {
  return L;
}

static void luat_openlibs(lua_State *L) {
    luaL_requiref(L, "msgbus", luaopen_msgbus, 1);
    lua_pop(L, 1);

    luaL_requiref(L, "rtos", luaopen_rtos, 1);
    lua_pop(L, 1);

    luaL_requiref(L, "sys", luaopen_sys, 1);
    lua_pop(L, 1);
    
    luaL_requiref(L, "timer", luaopen_timer, 1);
    lua_pop(L, 1);

    //#ifdef RT_USING_PIN
    luaL_requiref(L, "gpio", luaopen_gpio, 1);
    lua_pop(L, 1);
    //#endif
}

static int test_simple_core() {
  // Test A, 没有外部库的逻辑
  int re = 0;
  re = luaL_dostring(L, "print(_VERSION)");
  re = luaL_dostring(L, "local a = 1");
  re = luaL_dostring(L, "print(\"test1=====\") local a = 1\n local b=2\nprint(_G)\nprint(a+b)\nprint(sys)\nprint(_VERSION)");
    //    re = luaL_dostring(L, "print(\"test2=====\") local ab=1 \nprint(rtos.get_version()) print(rtos.get_memory_free())");
    //    re = luaL_dostring(L, "print(\"test3=====\") print(a - 1)");
    //    re = luaL_dostring(L, "print(\"test4=====\") print(rtos.get_memory_free()) collectgarbage(\"collect\") print(rtos.get_memory_free())");
    //    re = luaL_dostring(L, "print(\"test5=====\") print(rtos.timer_start(1, 3000)) print(rtos.receive(5000)) print(\"timer_get?\")");
    //    re = luaL_dostring(L, "print(\"test6=====\") local f = io.open(\"abc.log\", \"w\") print(f)");
    //    re = luaL_dostring(L, "print(_VERSION) print(\"sleep 2s\") timer.mdelay(2000) print(\"hi again\")");
    return re;
}

static int test_gpio_simple() {
        int re = luaL_dostring(L, "print(_VERSION)\n"
                   " local PA1=14\n"
                   " local PB7=27\n"
                   " gpio.setup(PA1,gpio.OUTPUT)\n"
                   " gpio.setup(PB7,gpio.INPUT, function() end)\n"
                   " print(PA1)\n"
                   " gpio.set(PA1, 0)\n"
                   " while 1 do\n"
                   "    gpio.set(PA1, 1)\n"
                   "    print(\"sleep 1s\")\n"
                   //"    print(gpio.get(PB7) == 1)\n"
                   "    timer.mdelay(1000)\n"

                   "    gpio.set(PA1, 0)\n"
                   "    print(\"sleep 1s\")\n"
                   //"    print(gpio.get(PB7) == 1)\n"
                   "    timer.mdelay(1000)\n"
                   "end\n"
                   );
        return re;
}

static int test_gpio_led() {
  int re = luaL_dostring(L, "print(_VERSION)\n"
                   " local PB18=23\n"
                   " local PB17=22\n"
                   " local PB16=21\n"
                   " gpio.setup(PB17,gpio.OUTPUT)\n"
                   " gpio.setup(PB16,gpio.OUTPUT)\n"
                   " gpio.setup(PB18,gpio.OUTPUT)\n"
                   " gpio.set(PB17, 0)\n"
                   " gpio.set(PB16, 0)\n"
                   " gpio.set(PB18, 0)\n"
                   " local t = 0 \n"
                   " while 1 do\n"
                   "    t = t+1"
                   "    gpio.set(PB17, t%1 > 0 and 1 or 0)\n"
                   "    gpio.set(PB16, t%3 > 0 and 1 or 0)\n"
                   "    gpio.set(PB18, t%7 > 0 and 1 or 0)\n"
                   "    print(\"sleep 1s - PB17\")\n"
                   "    timer.mdelay(1000)\n"
                   /*
                   "    gpio.set(PB17, 0)\n"
                   "    gpio.set(PB16, 1)\n"
                   "    gpio.set(PB18, 0)\n"
                   "    print(\"sleep 1s - PB16\")\n"
                   "    timer.mdelay(1000)\n"
                   
                   "    gpio.set(PB17, 0)\n"
                   "    gpio.set(PB16, 0)\n"
                   "    gpio.set(PB18, 1)\n"
                   "    print(\"sleep 1s - PB18\")\n"
                   "    timer.mdelay(1000)\n"
                   */
                   "end\n"
                   );
        return re;
}

static int test_timer_simple() {
          int re = luaL_dostring(L, "print(_VERSION)\n"
                   " while 1 do\n"
                   "    print(\"sleep 1s\")\n"
                   "    timer.mdelay(1000)\n"
                   "end\n"
                   );
        return re;
}

static int test_io_simple() {
  int re = luaL_dostring(L, "local f = io.open('/lua/main.lua')\n"
                            " print(f:read(64))\n"
                            " f:close()\n");
  return re;
}

static int test_load_fs() {
  return luaL_dofile(L, "/main.lua");
  /*
  FILE *f = fopen("/lua/main.lua", "r");
  if (f) {
    luat_printf("loading /lua/main.lua\n");
    fclose(f);
    return luaL_dofile(L, "/lua/main.lua");
  }
  else {
    luat_printf("not found /lua/main.lua\n");
  }
  return 0;
  */
}

static int pmain(lua_State *L) {
    int re = 0;
    //luat_print("luat_pmain!!!\n");
    // 加载系统库
    luaL_openlibs(L);

    // 加载本地库
    luat_openlibs(L);
    
    // 测试代码
    // re = test_core_simple();
    // re = test_gpio_simple();
    // re = test_gpio_led();
    // re = test_timer_simple();
    // re = test_io_simple();
    re = test_load_fs();

    if (re) {
        //luat_print("luaL_dostring  return re != 0\n");
        luat_print(lua_tostring(L, -1));
    }
    lua_pushboolean(L, 1);  /* signal no errors */
    return 1;
}

/*
** Prints an error message, adding the program name in front of it
** (if present)
*/
static void l_message (const char *pname, const char *msg) {
  if (pname) lua_writestringerror("%s: ", pname);
  lua_writestringerror("%s\n", msg);
}


/*
** Check whether 'status' is not OK and, if so, prints the error
** message on the top of the stack. It assumes that the error object
** is a string, as it was either generated by Lua or by 'msghandler'.
*/
static int report (lua_State *L, int status) {
  if (status != LUA_OK) {
    const char *msg = lua_tostring(L, -1);
    l_message("LUAT", msg);
    lua_pop(L, 1);  /* remove message */
  }
  return status;
}

static int panic (lua_State *L) {
  lua_writestringerror("PANIC: unprotected error in call to Lua API (%s)\n",
                        lua_tostring(L, -1));
  return 0;  /* return to Lua to abort */
}

int luat_main (int argc, char **argv, int _) {
  // 1. init filesystem
  luat_fs_init();

  // 2. init Lua State
  int status, result;
  L = lua_newstate(luat_heap_alloc, NULL);
  if (L == NULL) {
    l_message(argv[0], "cannot create state: not enough memory");
    return 1;
  }
  if (L) lua_atpanic(L, &panic);
  lua_pushcfunction(L, &pmain);  /* to call 'pmain' in protected mode */
  lua_pushinteger(L, argc);  /* 1st argument */
  lua_pushlightuserdata(L, argv); /* 2nd argument */
  status = lua_pcall(L, 2, 1, 0);  /* do the call */
  result = lua_toboolean(L, -1);  /* get result */
  report(L, status);
  //lua_close(L);
  return (result && status == LUA_OK) ? 0 : 2;
}
