/**
 * \file main.cpp
 * \author Matt Raykowski
 */

#include "stdpch.h"
#include <cstdio>
#include <nel/misc/types_nl.h>
#include "CDatabase.h"

#include "CUser.h"

int main(void) {
	nlinfo("Begin persistence test.");

	// connect to the database and make sure it succeeded.
	bool cnx=CDatabase::instance().connect("localhost","root","sp4nky","werewolf");
	if(!cnx) {
		nlinfo("Unsuccessful connection to database server.");
		exit(1);
	}

//	CDataResult *res=CDatabase::instance().query("select * from werewolf.users where userid=%d",1);
//	if(res->numRows()<=0)
//		printf("No rows returned when fetching.\n\r");

//	CDataRow *row=res->fetchRow();
//	if(row->isValid()) {
//		printf("Number of fields on this table: %d\n\r", row->getNumCols());
//
//		printf("Column Names: ");
//		for(uint idx=0 ; idx < row->getNumCols() ; idx++ ) {
//			printf("%s ",row->getColName(idx));
//		}
//		printf("\n\r");
//	}
	
//	while(row->isValid()) {
//		std::string uid=(*row)["userid"];
//		std::string uid=(*row)[0];
//		std::string uid=row->getColumn("userid");
//		std::string uid=row->getColumn(0);
//		uint userid=0;
//		if(uid!="")
//			userid=atoi(uid.c_str());
//
//		printf("Got a valid row: userid %d (%s) username: %s\n\r", userid, uid.c_str(), (*row)["username"].c_str());
//		row=res->fetchRow();
//	}

	CUser user;
	user.setUsername("mattr");
//	user.setPassword("banana");
	uint persid=user.query();
	user.load(persid);
	nlinfo("Persistence ID: %d Cookie: %s",persid, user.getUserCookie().c_str());

	if(user.getUserCookie() == "") {
		nlinfo("No cookie, lets set one.");
		user.setUserCookie("testcookie.");
		user.save();
	}

	if(user.getPassword()=="banana")
		user.setPassword("fubar");
	else
		user.setPassword("banana");
	user.save();

	CUser user2;
	user.setUsername("testuser2");
	user.setPassword("foobaar");
	uint pers2=user.query();
	if(pers2==0) {
		user.save();
	} else {
		;
	}

	// drop the database.
	CDatabase::instance().disconnect();
	nlinfo("Disconnected from database.\n\r");
}