/**
 * 
 */
package com.ironicentertainment.core;

/**
 * @author raykowski
 *
 */
public class User {
	private Integer _userId;
	private String _userName;
	private String _password;
	private String _privilege;
	
	public Integer getUserid() {
		return _userId;
	}
	
	public String getUsername() {
		return _userName;
	}
	
	public String getPasswd() {
		return _password;
	}
	
	public String getPrivilege() {
		return _privilege;
	}
	
	public void setUserid(Integer id) {
		_userId=id;
	}
	
	public void setUsername(String uname) {
		_userName=uname;
	}
	
	public void setPasswd(String pass) {
		_password=pass;
	}
	
	public void setPrivilege(String privs) {
		_privilege=privs;
	}
	
	public boolean checkPrivilege(String priv) {
		String [] str = _privilege.split(":");
		
		for(int i= 0 ; i<str.length ; i++) {
			if(str[i].equals(priv))
				return true;
		}
		return false;
	}
}
