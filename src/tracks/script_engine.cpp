		#include <stdarg.h> 
        #include <stdio.h> 
		#include <iostream>

		//#include "modes/world.hpp"

        #include "../../../dependencies/SQUIRREL3/include/squirrel.h"
        //#include <sqstdio.h> 
        //#include <sqstdaux.h> 
        #include "../../../dependencies/SQUIRREL3/include/sqstdio.h"
        #include "../../../dependencies/SQUIRREL3/include/sqstdaux.h"
		#include "script_engine.hpp"

  /*      #ifdef _MSC_VER
        #pragma comment (lib ,"squirrel.lib")
        #pragma comment (lib ,"sqstdlib.lib")
        #endif
*/

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

		const SQChar* call_foo11(HSQUIRRELVM v, int n,float f,const SQChar *s)
		//std::string call_foo11(HSQUIRRELVM v, int n,float f,const SQChar *s)
        {
			/*
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
				return out; */
				const SQChar* outvalue;

				const SQChar *program = "return\"hwaaatt\"";
 
				if (SQ_FAILED(sq_compilebuffer(v, program, 
                                  // sizeof(SQChar) * strlen(program), 
                                   sizeof(SQChar) * 16, 
                                   "program", SQTrue))) {
				return NULL;
				}
				sq_pushroottable(v); //push the root table(were the globals of the script will be stored)
				if (SQ_FAILED(sq_call(v, 1, SQTrue, SQTrue))) {

				return NULL;
				}
				sq_getstring(v,-1,&outvalue);
				return outvalue;
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
		//std::string call_foo111(HSQUIRRELVM v, int n,float f,const SQChar *s)
		const SQChar* call_foo111(HSQUIRRELVM v, int n,float f,const SQChar *s)
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
	/*		HSQUIRRELVM v; 
                v = sq_open(1024); // creates a VM with initial stack size 1024 


                sqstd_seterrorhandlers(v);


                sq_setprintfunc(v, printfunc, NULL); //sets the print function
				//out = "came here1";
				std::string output;
                sq_pushroottable(v); //push the root table(were the globals of the script will be stored)
                //if(SQ_FAILED(sqstd_dofile(v, _SC("test.nut"), 0, 1))) // also prints syntax errors if any
				sq_pushstring(v,_SC("foo"),-1);
				if(SQ_SUCCEEDED(sqstd_dofile(v, _SC("test.nut"), 0, 1))) // also prints syntax errors if any 
                {
						//  call_foo(v);
                        output = call_foo111(v,1,2.5,_SC("teststring"));
						//out = "came here2";
                }

				if (output!="wow")outval = output;
				else outval = "wow";
                sq_pop(v,1); //pops the root table
                sq_close(v); 
				char a;
				//out = output;

				//std::cin>>a;
				//std::cout<<a;
				//std::cout<<"woot";
                //return 0; 
*/
		}
		std::string convert(const SQChar* in){
			return (std::string)in;
		}
		std::string ScriptEngine::doit(){
			                
               // return 0; 
			HSQUIRRELVM v; 
                v = sq_open(1024); // creates a VM with initial stack size 1024 


                sqstd_seterrorhandlers(v);


                sq_setprintfunc(v, printfunc, NULL); //sets the print function

				//std::string out;
				const SQChar* out;


                sq_pushroottable(v); //push the root table(were the globals of the script will be stored)
				//sq_pushstring(v,_SC("foo"),-1);
               // if(SQ_FAILED(sqstd_dofile(v, _SC("test.nut"), 0, 1))) // also prints syntax errors if any 
               // {
						//  call_foo(v);+
						//  call_foo(v,1,2.5,_SC("teststring"));
                        out = call_foo11(v,5,2.5,_SC("teststring"));
						//if (out=="wow")call_foo111(v,1,1.1,("weh"));
              //  }

				/*
				To call a squirrel function it is necessary to push the function in the stack followed by the parameters and then call the function sq_call. The function will pop the parameters and push the return value if the last sq_call parameter is >0.

				sq_pushroottable(v);
				sq_pushstring(v,“foo”,-1);
				sq_get(v,-2); //get the function from the root table
				sq_pushroottable(v); //’this’ (function environment object)
				sq_pushinteger(v,1);
				sq_pushfloat(v,2.0);
				sq_pushstring(v,”three”,-1);
				sq_call(v,4,SQFalse);
				sq_pop(v,2); //pops the roottable and the function
		
				this is equivalent to the following Squirrel code

				foo(1,2.0,”three”);
				If a runtime error occurs (or a exception is thrown) during the squirrel code execution the sq_call will fail.
				*/
				//sq_pushstring(v,_SC("wow"),-1);
				//sq_getstring(v,-1,&out);
				//std::string great = "wow123";
				//if (convert(out) == "wow")great = "wow";
                sq_pop(v,1); //pops the root table
                sq_close(v); 
				//char a;
				//std::cin>>a;
				//std::cout<<a;
				//OutputDebugString("a");
				/*va_list arglist;
				std::cout<<"woot";
				va_start(arglist, "wat"); 
				scvprintf("wat",arglist); 
				va_end(arglist); 
				*/
				return convert(out);

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
