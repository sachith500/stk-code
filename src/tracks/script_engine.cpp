		#include <stdarg.h> 
        #include <stdio.h> 
		#include <iostream>

		//#include "modes/world.hpp"

        #include <squirrel.h> 
        #include <sqstdio.h> 
        #include <sqstdaux.h> 
		#include "script_engine.hpp"

        #ifdef _MSC_VER
        #pragma comment (lib ,"squirrel.lib")
        #pragma comment (lib ,"sqstdlib.lib")
        #endif


        #ifdef SQUNICODE 
        #define scvprintf vwprintf 
        #else 
        #define scvprintf vprintf 
        #endif 

        void printfunc(HSQUIRRELVM v, const SQChar *s, ...) 
        { 
                va_list arglist; 
                va_start(arglist, s); 
                scvprintf(s, arglist); 
                va_end(arglist); 
        } 

		std::string call_foo11(HSQUIRRELVM v, int n,float f,const SQChar *s)
        {
				const SQChar*  out = NULL;
				std::string output;
                int top = sq_gettop(v); //saves the stack size before the call
                sq_pushroottable(v); //pushes the global table
                sq_pushstring(v,_SC("foo"),-1);
                if(SQ_SUCCEEDED(sq_get(v,-2))) { //gets the field 'foo' from the global table
                        sq_pushroottable(v); //push the 'this' (in this case is the global table)
                        sq_pushinteger(v,n); 
                        sq_pushfloat(v,f);
                        sq_pushstring(v,s,-1);
                        sq_call(v,4,1,0); //calls the function the 3rd one causes the return value to be pushed.
						//sq_pushstring(v,"wow",-1);
						sq_getstring(v,-1,&out);
						
                }
                sq_settop(v,top); //restores the original stack size
				return out;
        }
		std::string call_foo(HSQUIRRELVM v, int n,float f,const SQChar *s)
        {
				const SQChar*  out = NULL;
				std::string output;
                int top = sq_gettop(v); //saves the stack size before the call
                sq_pushroottable(v); //pushes the global table
                sq_pushstring(v,_SC("foo"),-1);
                if(SQ_SUCCEEDED(sq_get(v,-2))) { //gets the field 'foo' from the global table
                        sq_pushroottable(v); //push the 'this' (in this case is the global table)
                        sq_pushinteger(v,n); 
                        sq_pushfloat(v,f);
                        sq_pushstring(v,s,-1);
                        sq_call(v,4,1,0); //calls the function the 3rd one causes the return value to be pushed.
						//sq_pushstring(v,"wow",-1);
						sq_getstring(v,-1,&out);
						
                }
				sq_pushstring(v,"wow",-1);
				sq_getstring(v,-1,&out);
				if (out=="wow")out = "wow";
                sq_settop(v,top); //restores the original stack size
				return out;
        }
        void call_foo3(HSQUIRRELVM v, int n,float f,const SQChar *s)
        {
                int top = sq_gettop(v); //saves the stack size before the call
                sq_pushroottable(v); //pushes the global table
                sq_pushstring(v,_SC("foo"),-1);
                if(SQ_SUCCEEDED(sq_get(v,-2))) { //gets the field 'foo' from the global table
                        sq_pushroottable(v); //push the 'this' (in this case is the global table)
                        sq_pushinteger(v,n); 
                        sq_pushfloat(v,f);
                        sq_pushstring(v,s,-1);
                        sq_call(v,4,0,0); //calls the function 
                }
                sq_settop(v,top); //restores the original stack size
        }
		void call_foo2(HSQUIRRELVM v)
        {
			int wow;
                int top = sq_gettop(v); //saves the stack size before the call
                sq_pushroottable(v); //pushes the global table
                sq_pushstring(v,_SC("foo"),-1);
                if(SQ_SUCCEEDED(sq_get(v,-2))) { //gets the field 'foo' from the global table
                        sq_pushroottable(v); //push the 'this' (in this case is the global table)
                     //   sq_pushinteger(v,n); 
                      //  sq_pushfloat(v,f);
                      //  sq_pushstring(v,s,-1);
                        sq_call(v,1,0,0); //calls the function 

                }
                sq_settop(v,top); //restores the original stack size
        }

		
		ScriptEngine::ScriptEngine(){
			HSQUIRRELVM v; 
                v = sq_open(1024); // creates a VM with initial stack size 1024 


                sqstd_seterrorhandlers(v);


                sq_setprintfunc(v, printfunc, NULL); //sets the print function
				out = "came here1";
				std::string output;
                sq_pushroottable(v); //push the root table(were the globals of the script will be stored)
                //if(SQ_FAILED(sqstd_dofile(v, _SC("test.nut"), 0, 1))) // also prints syntax errors if any 
				if(SQ_SUCCEEDED(sqstd_dofile(v, _SC("test.nut"), 0, 1))) // also prints syntax errors if any 
                {
						//  call_foo(v);
                        output = call_foo(v,1,2.5,_SC("teststring"));
						out = "came here2";
                }

				if (out!="wow")out = output;
				else out = "wow";
                sq_pop(v,1); //pops the root table
                sq_close(v); 
				char a;
				//out = output;

				//std::cin>>a;
				//std::cout<<a;
				//std::cout<<"woot";
                //return 0; 

		}
    /*    int main(int argc, char* argv[]) 
        { 
                HSQUIRRELVM v; 
                v = sq_open(1024); // creates a VM with initial stack size 1024 


                sqstd_seterrorhandlers(v);


                sq_setprintfunc(v, printfunc, NULL); //sets the print function


                sq_pushroottable(v); //push the root table(were the globals of the script will be stored)
                if(SQ_SUCCEEDED(sqstd_dofile(v, _SC("test.nut"), 0, 1))) // also prints syntax errors if any 
                {
						//  call_foo(v);
                        call_foo(v,1,2.5,_SC("teststring"));
                }

				
                sq_pop(v,1); //pops the root table
                sq_close(v); 
				char a;
				std::cin>>a;
				std::cout<<a;
				std::cout<<"woot";
                return 0; 
        } */