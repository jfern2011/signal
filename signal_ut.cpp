#include <iostream>
#include <string>

#include "Signal.h"

class MemPtrTest
{
public:

	MemPtrTest()
	{
	}

	~MemPtrTest()
	{
	}

	void bool_constStringRef(bool b, const std::string& str)
	{
		std::cout << __PRETTY_FUNCTION__;
		std::cout << ": b = " << b
			<< ", str = " << str << std::endl;
	}

	void bool_string(bool b, std::string str)
	{
		std::cout << __PRETTY_FUNCTION__;
		std::cout << ": b = " << b
			<< ", str = " << str << std::endl;
	}

	void bool_stringRef(bool b, std::string& str)
	{
		std::cout << __PRETTY_FUNCTION__;
		std::cout << ": b = " << b
			<< ", str = " << str << std::endl;
	}

	void bool_stringRRef(bool b, std::string&& str)
	{
		std::cout << __PRETTY_FUNCTION__;
		std::cout << ": b = " << b
			<< ", str = " << str << std::endl;
	}

	void int_constStringRef_const(int i, const std::string& str) const
	{
		std::cout << __PRETTY_FUNCTION__;
		std::cout << ": i = " << i
			<< ", str = " << str << std::endl;
	}

	void int_charPtr_voidPtr(int i, char* str1, void* str2)
	{
		std::cout << __PRETTY_FUNCTION__;
		std::cout << ": i = " << i
			<< ", str1 = " << str1
			<< ", str2 = " << str2 << std::endl;
	}

	void int_constCharPtr_constVoidPtr(int i, const char* str1,
		                               const void* str2)
	{
		std::cout << __PRETTY_FUNCTION__;
		std::cout << ": i = " << i
			<< ", str1 = " << str1
			<< ", str2 = " << str2 << std::endl;
	}

	MemPtrTest& void_()
	{
		std::cout << __PRETTY_FUNCTION__ << std::endl;
		return *this;
	}

	void void_const() const
	{
		std::cout << __PRETTY_FUNCTION__ << std::endl;
	}
};

class FcnPtrTest
{
public:

	FcnPtrTest()
	{
	}

	~FcnPtrTest()
	{
	}

	static void bool_constStringRef(bool b, const std::string& str)
	{
		std::cout << __PRETTY_FUNCTION__;
		std::cout << ": b = " << b
			<< ", str = " << str << std::endl;
	}

	static void bool_string(bool b, std::string str)
	{
		std::cout << __PRETTY_FUNCTION__;
		std::cout << ": b = " << b
			<< ", str = " << str << std::endl;
	}

	static void bool_stringRef(bool b, std::string& str)
	{
		std::cout << __PRETTY_FUNCTION__;
		std::cout << ": b = " << b
			<< ", str = " << str << std::endl;
	}

	static void bool_stringRRef(bool b, std::string&& str)
	{
		std::cout << __PRETTY_FUNCTION__;
		std::cout << ": b = " << b
			<< ", str = " << str << std::endl;
	}

	static void int_charPtr_voidPtr(int i, char* str1, void* str2)
	{
		std::cout << __PRETTY_FUNCTION__;
		std::cout << ": i = " << i
			<< ", str1 = " << str1
			<< ", str2 = " << str2 << std::endl;
	}

	static void int_constCharPtr_constVoidPtr(int i, const char* str1,
		                               const void* str2)
	{
		std::cout << __PRETTY_FUNCTION__;
		std::cout << ": i = " << i
			<< ", str1 = " << str1
			<< ", str2 = " << str2 << std::endl;
	}

	static FcnPtrTest void_()
	{
		std::cout << __PRETTY_FUNCTION__ << std::endl;
		return FcnPtrTest();
	}
};

class SignalTest
{
public:

	SignalTest()
	{
	}

	~SignalTest()
	{
	}

	bool memPtrTest1()
	{
		MemPtrTest test;
		Signal::mem_ptr<void,MemPtrTest,bool,const std::string&>
			mem_ptr1(test);
		Signal::mem_ptr<void,MemPtrTest,bool,const std::string&>
			mem_ptr2(test,&MemPtrTest::bool_constStringRef);

		mem_ptr1.attach(&MemPtrTest::bool_constStringRef);

		if (!mem_ptr1.is_connected())
		{
			return false;
		}

		if (!mem_ptr2.is_connected())
		{
			return false;
		}

		mem_ptr1.raise(true, "hello");
		mem_ptr2.raise(false, "goodbye");

		mem_ptr1.bind(true, "hiya");
		mem_ptr2.bind(false, "see ya");

		mem_ptr1.raise();
		mem_ptr2.raise();

		mem_ptr1.detach();
		mem_ptr2.detach();

		if (mem_ptr1.is_connected())
		{
			return false;
		}

		if (mem_ptr2.is_connected())
		{
			return false;
		}

		return true;
	}

	bool memPtrTest2()
	{
		MemPtrTest test;
		Signal::mem_ptr<void,MemPtrTest,bool,std::string>
			mem_ptr1(test);
		Signal::mem_ptr<void,MemPtrTest,bool,std::string>
			mem_ptr2(test,&MemPtrTest::bool_string);

		mem_ptr1.attach(&MemPtrTest::bool_string);

		if (!mem_ptr1.is_connected())
		{
			return false;
		}

		if (!mem_ptr2.is_connected())
		{
			return false;
		}

		mem_ptr1.raise(true, "hello");
		mem_ptr2.raise(false, "goodbye");

		mem_ptr1.bind(true, "hiya");
		mem_ptr2.bind(false, "see ya");

		mem_ptr1.raise();
		mem_ptr2.raise();

		mem_ptr1.detach();
		mem_ptr2.detach();

		if (mem_ptr1.is_connected())
		{
			return false;
		}

		if (mem_ptr2.is_connected())
		{
			return false;
		}

		return true;
	}

	bool memPtrTest3()
	{
		MemPtrTest test;
		Signal::mem_ptr<void,MemPtrTest,bool,std::string&>
			mem_ptr1(test);
		Signal::mem_ptr<void,MemPtrTest,bool,std::string&>
			mem_ptr2(test,&MemPtrTest::bool_stringRef);

		mem_ptr1.attach(&MemPtrTest::bool_stringRef);

		if (!mem_ptr1.is_connected())
		{
			return false;
		}

		if (!mem_ptr2.is_connected())
		{
			return false;
		}

		std::string str1 = "hello";
		std::string str2 = "goodbye";
		mem_ptr1.raise(true, str1);
		mem_ptr2.raise(false, str2);

		str1 = "hiya";
		str2 = "see ya";
		mem_ptr1.bind(true, str1);
		mem_ptr2.bind(false, str2);

		mem_ptr1.raise();
		mem_ptr2.raise();

		mem_ptr1.detach();
		mem_ptr2.detach();

		if (mem_ptr1.is_connected())
		{
			return false;
		}

		if (mem_ptr2.is_connected())
		{
			return false;
		}

		return true;
	}

	bool memPtrTest4()
	{
		MemPtrTest test;
		Signal::mem_ptr<void,MemPtrTest,int,const std::string&>
			mem_ptr1(test);
		Signal::mem_ptr<void,MemPtrTest,int,const std::string&>
			mem_ptr2(test,&MemPtrTest::int_constStringRef_const);

		mem_ptr1.attach(&MemPtrTest::int_constStringRef_const);

		if (!mem_ptr1.is_connected())
		{
			return false;
		}

		if (!mem_ptr2.is_connected())
		{
			return false;
		}

		mem_ptr1.raise(3,"heyo!");
		mem_ptr2.raise(4,"peace out!");

		mem_ptr1.bind(3,"heyo!");
		mem_ptr2.bind(4,"peace out!");

		mem_ptr1.raise();
		mem_ptr2.raise();

		mem_ptr1.detach();
		mem_ptr2.detach();

		if (mem_ptr1.is_connected())
		{
			return false;
		}

		if (mem_ptr2.is_connected())
		{
			return false;
		}

		return true;
	}

	bool memPtrTest5()
	{
		MemPtrTest test;
		Signal::mem_ptr<void,MemPtrTest,int,char*,void*>
			mem_ptr1(test);
		Signal::mem_ptr<void,MemPtrTest,int,char*,void*>
			mem_ptr2(test,&MemPtrTest::int_charPtr_voidPtr);

		mem_ptr1.attach(&MemPtrTest::int_charPtr_voidPtr);

		if (!mem_ptr1.is_connected())
		{
			return false;
		}

		if (!mem_ptr2.is_connected())
		{
			return false;
		}

		char
		str1[] = "hello",
		str2[] = "how's it going?",
		str3[] = "goodbye",
		str4[] = "take care",
		str5[] = "hiya",
		str6[] = "what's up?",
		str7[] = "see ya",
		str8[] = "have a good day!";

		mem_ptr1.raise(1, str1, str2);
		mem_ptr2.raise(2, str3, str4);

		mem_ptr1.bind(1, str5, str6);
		mem_ptr2.bind(2, str7, str8);

		mem_ptr1.raise();
		mem_ptr2.raise();

		mem_ptr1.detach();
		mem_ptr2.detach();

		if (mem_ptr1.is_connected())
		{
			return false;
		}

		if (mem_ptr2.is_connected())
		{
			return false;
		}

		return true;
	}

	bool memPtrTest6()
	{
		MemPtrTest test;
		Signal::mem_ptr<void,MemPtrTest,int,const char*,const void*>
			mem_ptr1(test);
		Signal::mem_ptr<void,MemPtrTest,int,const char*,const void*>
			mem_ptr2(test,&MemPtrTest::int_constCharPtr_constVoidPtr);

		mem_ptr1.attach(&MemPtrTest::int_constCharPtr_constVoidPtr);

		if (!mem_ptr1.is_connected())
		{
			return false;
		}

		if (!mem_ptr2.is_connected())
		{
			return false;
		}

		mem_ptr1.raise(1, "hello", "how's it going?");
		mem_ptr2.raise(2, "goodbye", "take care");

		mem_ptr1.bind(1, "hiya", "what's up?");
		mem_ptr2.bind(2, "see ya", "have a good day!");

		mem_ptr1.raise();
		mem_ptr2.raise();

		mem_ptr1.detach();
		mem_ptr2.detach();

		if (mem_ptr1.is_connected())
		{
			return false;
		}

		if (mem_ptr2.is_connected())
		{
			return false;
		}

		return true;
	}

	bool memPtrTest7()
	{
		MemPtrTest test;
		Signal::mem_ptr<MemPtrTest&,MemPtrTest,void>
			mem_ptr1(test);
		Signal::mem_ptr<MemPtrTest&,MemPtrTest,void>
			mem_ptr2(test,&MemPtrTest::void_);

		mem_ptr1.attach(&MemPtrTest::void_);

		if (!mem_ptr1.is_connected())
		{
			return false;
		}

		if (!mem_ptr2.is_connected())
		{
			return false;
		}

		MemPtrTest obj1 = mem_ptr1.raise();
		MemPtrTest obj2 = mem_ptr2.raise();

		mem_ptr1.detach();
		mem_ptr2.detach();

		if (mem_ptr1.is_connected())
		{
			return false;
		}

		if (mem_ptr2.is_connected())
		{
			return false;
		}

		return true;
	}

	bool memPtrTest8()
	{
		MemPtrTest test;
		Signal::mem_ptr<void,MemPtrTest,void>
			mem_ptr1(test);
		Signal::mem_ptr<void,MemPtrTest,void>
			mem_ptr2(test,&MemPtrTest::void_const);

		mem_ptr1.attach(&MemPtrTest::void_const);

		if (!mem_ptr1.is_connected())
		{
			return false;
		}

		if (!mem_ptr2.is_connected())
		{
			return false;
		}

		mem_ptr1.raise();
		mem_ptr2.raise();

		mem_ptr1.detach();
		mem_ptr2.detach();

		if (mem_ptr1.is_connected())
		{
			return false;
		}

		if (mem_ptr2.is_connected())
		{
			return false;
		}

		return true;
	}

	void runMemPtrSuite()
	{
		if (!memPtrTest1())
			std::cout << "failed test 1" << std::endl;
		else
			std::cout << "passed test 1" << std::endl;

		if (!memPtrTest2())
			std::cout << "failed test 2" << std::endl;
		else
			std::cout << "passed test 2" << std::endl;

		if (!memPtrTest3())
			std::cout << "failed test 3" << std::endl;
		else
			std::cout << "passed test 3" << std::endl;

		if (!memPtrTest4())
			std::cout << "failed test 4" << std::endl;
		else
			std::cout << "passed test 4" << std::endl;

		if (!memPtrTest5())
			std::cout << "failed test 5" << std::endl;
		else
			std::cout << "passed test 5" << std::endl;

		if (!memPtrTest6())
			std::cout << "failed test 6" << std::endl;
		else
			std::cout << "passed test 6" << std::endl;

		if (!memPtrTest7())
			std::cout << "failed test 7" << std::endl;
		else
			std::cout << "passed test 7" << std::endl;

		if (!memPtrTest8())
			std::cout << "failed test 8" << std::endl;
		else
			std::cout << "passed test 8" << std::endl;
	}

	//=================================================================

	bool fcnPtrTest1()
	{
		Signal::fcn_ptr<void,bool,const std::string&>
			fcn_ptr1;
		Signal::fcn_ptr<void,bool,const std::string&>
			fcn_ptr2(&FcnPtrTest::bool_constStringRef);

		fcn_ptr1.attach(&FcnPtrTest::bool_constStringRef);

		if (!fcn_ptr1.is_connected())
		{
			return false;
		}

		if (!fcn_ptr2.is_connected())
		{
			return false;
		}

		fcn_ptr1.raise(true, "hello");
		fcn_ptr2.raise(false, "goodbye");

		fcn_ptr1.bind(true, "hiya");
		fcn_ptr2.bind(false, "see ya");

		fcn_ptr1.raise();
		fcn_ptr2.raise();

		fcn_ptr1.detach();
		fcn_ptr2.detach();

		if (fcn_ptr1.is_connected())
		{
			return false;
		}

		if (fcn_ptr2.is_connected())
		{
			return false;
		}

		return true;
	}

	bool fcnPtrTest2()
	{
		Signal::fcn_ptr<void,bool,std::string>
			fcn_ptr1;
		Signal::fcn_ptr<void,bool,std::string>
			fcn_ptr2(&FcnPtrTest::bool_string);

		fcn_ptr1.attach(&FcnPtrTest::bool_string);

		if (!fcn_ptr1.is_connected())
		{
			return false;
		}

		if (!fcn_ptr2.is_connected())
		{
			return false;
		}

		fcn_ptr1.raise(true, "hello");
		fcn_ptr2.raise(false, "goodbye");

		fcn_ptr1.bind(true, "hiya");
		fcn_ptr2.bind(false, "see ya");

		fcn_ptr1.raise();
		fcn_ptr2.raise();

		fcn_ptr1.detach();
		fcn_ptr2.detach();

		if (fcn_ptr1.is_connected())
		{
			return false;
		}

		if (fcn_ptr2.is_connected())
		{
			return false;
		}

		return true;
	}

	bool fcnPtrTest3()
	{
		Signal::fcn_ptr<void,bool,std::string&>
			fcn_ptr1;
		Signal::fcn_ptr<void,bool,std::string&>
			fcn_ptr2(&FcnPtrTest::bool_stringRef);

		fcn_ptr1.attach(&FcnPtrTest::bool_stringRef);

		if (!fcn_ptr1.is_connected())
		{
			return false;
		}

		if (!fcn_ptr2.is_connected())
		{
			return false;
		}

		std::string str1 = "hello";
		std::string str2 = "goodbye";
		fcn_ptr1.raise(true, str1);
		fcn_ptr2.raise(false, str2);

		str1 = "hiya";
		str2 = "see ya";
		fcn_ptr1.bind(true, str1);
		fcn_ptr2.bind(false, str2);

		fcn_ptr1.raise();
		fcn_ptr2.raise();

		fcn_ptr1.detach();
		fcn_ptr2.detach();

		if (fcn_ptr1.is_connected())
		{
			return false;
		}

		if (fcn_ptr2.is_connected())
		{
			return false;
		}

		return true;
	}

	bool fcnPtrTest4()
	{
		Signal::fcn_ptr<void,int,char*,void*>
			fcn_ptr1;
		Signal::fcn_ptr<void,int,char*,void*>
			fcn_ptr2(&FcnPtrTest::int_charPtr_voidPtr);

		fcn_ptr1.attach(&FcnPtrTest::int_charPtr_voidPtr);

		if (!fcn_ptr1.is_connected())
		{
			return false;
		}

		if (!fcn_ptr2.is_connected())
		{
			return false;
		}

		char
		str1[] = "hello",
		str2[] = "how's it going?",
		str3[] = "goodbye",
		str4[] = "take care",
		str5[] = "hiya",
		str6[] = "what's up?",
		str7[] = "see ya",
		str8[] = "have a good day!";

		fcn_ptr1.raise(1, str1, str2);
		fcn_ptr2.raise(2, str3, str4);

		fcn_ptr1.bind(1, str5, str6);
		fcn_ptr2.bind(2, str7, str8);

		fcn_ptr1.raise();
		fcn_ptr2.raise();

		fcn_ptr1.detach();
		fcn_ptr2.detach();

		if (fcn_ptr1.is_connected())
		{
			return false;
		}

		if (fcn_ptr2.is_connected())
		{
			return false;
		}

		return true;
	}

	bool fcnPtrTest5()
	{
		Signal::fcn_ptr<void,int,const char*,const void*>
			fcn_ptr1;
		Signal::fcn_ptr<void,int,const char*,const void*>
			fcn_ptr2(&FcnPtrTest::int_constCharPtr_constVoidPtr);

		fcn_ptr1.attach(&FcnPtrTest::int_constCharPtr_constVoidPtr);

		if (!fcn_ptr1.is_connected())
		{
			return false;
		}

		if (!fcn_ptr2.is_connected())
		{
			return false;
		}

		fcn_ptr1.raise(1, "hello", "how's it going?");
		fcn_ptr2.raise(2, "goodbye", "take care");

		fcn_ptr1.bind(1, "hiya", "what's up?");
		fcn_ptr2.bind(2, "see ya", "have a good day!");

		fcn_ptr1.raise();
		fcn_ptr2.raise();

		fcn_ptr1.detach();
		fcn_ptr2.detach();

		if (fcn_ptr1.is_connected())
		{
			return false;
		}

		if (fcn_ptr2.is_connected())
		{
			return false;
		}

		return true;
	}

	bool fcnPtrTest6()
	{
		Signal::fcn_ptr<FcnPtrTest,void>
			fcn_ptr1;
		Signal::fcn_ptr<FcnPtrTest,void>
			fcn_ptr2(&FcnPtrTest::void_);

		fcn_ptr1.attach(&FcnPtrTest::void_);

		if (!fcn_ptr1.is_connected())
		{
			return false;
		}

		if (!fcn_ptr2.is_connected())
		{
			return false;
		}

		FcnPtrTest obj1 = fcn_ptr1.raise();
		FcnPtrTest obj2 = fcn_ptr2.raise();

		fcn_ptr1.detach();
		fcn_ptr2.detach();

		if (fcn_ptr1.is_connected())
		{
			return false;
		}

		if (fcn_ptr2.is_connected())
		{
			return false;
		}

		return true;
	}

	void runFcnPtrSuite()
	{
		if (!fcnPtrTest1())
			std::cout << "failed test 1" << std::endl;
		else
			std::cout << "passed test 1" << std::endl;

		if (!fcnPtrTest2())
			std::cout << "failed test 2" << std::endl;
		else
			std::cout << "passed test 2" << std::endl;

		if (!fcnPtrTest3())
			std::cout << "failed test 3" << std::endl;
		else
			std::cout << "passed test 3" << std::endl;

		if (!fcnPtrTest4())
			std::cout << "failed test 4" << std::endl;
		else
			std::cout << "passed test 4" << std::endl;

		if (!fcnPtrTest5())
			std::cout << "failed test 5" << std::endl;
		else
			std::cout << "passed test 5" << std::endl;

		if (!fcnPtrTest6())
			std::cout << "failed test 6" << std::endl;
		else
			std::cout << "passed test 6" << std::endl;
	}
};

class SignalTestSuite
{
public:

	SignalTestSuite()
	{
	}

	~SignalTestSuite()
	{
	}

	bool runTest1()
	{
		MemPtrTest mpTest;

		Signal::Signal<void,bool,const std::string&>
			sig1;
		Signal::Signal<void,int,const char*,const void*>
			sig2(&FcnPtrTest::int_constCharPtr_constVoidPtr);

		Signal::Signal<void,int,char*,void*>
			sig3(mpTest,&MemPtrTest::int_charPtr_voidPtr);

		Signal::Signal<void,int,const std::string&>
			sig4(mpTest,&MemPtrTest::int_constStringRef_const);

		if ( sig1.is_connected() ||
			!sig2.is_connected() ||
			!sig3.is_connected() ||
			!sig4.is_connected())
			return false;
		
		if (!sig1.attach(&FcnPtrTest::bool_constStringRef))
			return false;

		if (!sig1.is_connected())
			return false;

		if (!sig3.detach() ||
			!sig3.attach(mpTest,&MemPtrTest::int_charPtr_voidPtr))
			return false;

		if (!sig4.detach() || 
			!sig4.attach(mpTest,&MemPtrTest::int_constStringRef_const))
			return false;

		if (!sig3.attach(&MemPtrTest::int_charPtr_voidPtr))
			return false;

		if (!sig4.attach(&MemPtrTest::int_constStringRef_const))
			return false;

		sig1.raise(true,"test 1");
		sig2.raise(5,"test 2.1", "test 2.2");

		char str1[] = "test 3";
		char str2[] = "test 4";
		sig3.raise(6,str1,str2);
		sig4.raise(7,"test 5");

		sig1.bind(false,"test 6");
		sig2.bind(8,"test 7", "test 8");

		char str3[] = "test 9";
		char str4[] = "test 10";
		sig3.bind(9,str3,str4);
		sig4.bind(10,"test 11");

		sig1.raise();
		sig2.raise();
		sig3.raise();
		sig4.raise();

		return true;
	}

	bool runTest2()
	{
		MemPtrTest mpTest;

		Signal::Signal<FcnPtrTest,void>
			sig1;

		Signal::Signal<MemPtrTest&,void>
			sig2(mpTest,&MemPtrTest::void_);

		Signal::Signal<void,void>
			sig3(mpTest,&MemPtrTest::void_const);

		Signal::Signal<FcnPtrTest,void>
			sig4(&FcnPtrTest::void_);

		if ( sig1.is_connected() ||
			!sig2.is_connected() ||
			!sig3.is_connected() ||
			!sig4.is_connected())
			return false;
		
		if (!sig1.attach(&FcnPtrTest::void_))
			return false;

		if (!sig1.is_connected())
			return false;

		if (!sig2.detach() ||
			!sig2.attach(mpTest, &MemPtrTest::void_))
			return false;

		if (!sig2.attach(&MemPtrTest::void_))
			return false;

		if (!sig3.detach() || 
			!sig3.attach(mpTest,&MemPtrTest::void_const))
			return false;

		if (!sig3.attach(&MemPtrTest::void_const))
			return false;

		FcnPtrTest test1 = sig1.raise();
		MemPtrTest test2 = sig2.raise();
		sig3.raise();

		return true;
	}

	void run()
	{
		if (!runTest1())
			std::cout << "Failed test suite 1" << std::endl;
		else
			std::cout << "Passed test suite 1" << std::endl;

		if (!runTest2())
			std::cout << "Failed test suite 2" << std::endl;
		else
			std::cout << "Passed test suite 2" << std::endl;
	}
};

int main()
{
	SignalTest test1, test2;
	test1.runMemPtrSuite();
	test2.runFcnPtrSuite();

	SignalTestSuite suite;
	suite.run();

	return 0;
}