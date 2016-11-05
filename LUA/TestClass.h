#ifndef TEST_CLASS_H
#define TEST_CLASS_H

#include "./Strings/MyStringAnsi.h"

class Account2 {
public:

	Account2(double balance) { m2 = 30; m_balance = balance; }
	void deposit2(double amount) { m_balance += amount + m2; }
	void withdraw2(double amount) { m_balance -= amount; }
	double balance2(void) { return m_balance; }
	void Print0() { printf("Account2 EMPTY\n"); }
private:
	double m_balance;
	double m2;
};

class Account {
public:

	double val = 148;
	MyStringAnsi xx = "ahoj aatr";
	Account2 * cc = new Account2(1);
	//Account * cc = new Account(1);

	Account(double balance) { m_balance = balance;; }
	//Account(const Account & c) { m_balance = c.m_balance; printf("CC\n"); };
	//Account(Account && c) {printf("MC\n"); };

	void deposit(double amount) { m_balance += amount; }
	void withdraw(double amount) { m_balance -= amount; }
	double balance(void)
	{
		return m_balance;
	}
	double balance2(int s) { printf("MUL: %i\n", s); return s * m_balance; }
	void Print0()
	{
		printf("STR0: EMPTY\n");
		val = 77;
	}

	void Print0(double vl, double o)
	{
		printf("STR0: overload %f %f\n", vl, o);		
	}

	void Print1(const short& xx)
	{
		printf("STR1: EMPTY %i\n", xx);
	}

	void Print2(MyStringAnsi tt, int ii, int jj)
	{
		printf("STR2: %s %i %i\n", tt.c_str(), ii, jj);
	}

	//void Print3(const MyStringAnsi & tt, int ii, int jj)
	void Print3(const MyStringAnsi & tt, int ii, int jj)
	{
		printf("STR3: %s %i %i\n", tt.c_str(), ii, jj);
	}

	int Print4()
	{
		printf("STR4\n");
		return 5;
	}

	int Print5(int x)
	{
		printf("STR5 %i\n", x);
		return x + 5;
	}


	void Print6(Account * c)
	{
		c->m_balance *= -1;
		printf("STR6: %f\n", c->m_balance);
	}

	void Print7(Account & c)
	{
		c.m_balance *= -1;
		printf("STR7: %f\n", c.m_balance);
	}

private:
	double m_balance;
};

#endif
