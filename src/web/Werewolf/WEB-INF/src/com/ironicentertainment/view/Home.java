/**
 * 
 */
package com.ironicentertainment.view;

import org.apache.tapestry.html.BasePage;

/**
 * @author raykowski
 *
 */
public class Home extends BasePage {
	String greeting = "Greetings from the locals!";

	/**
	 * This actually defines an abstract property that is set in the Home.page file by spring.
	 * The abstract getter is required so you can do stuff like greetingService.greeting in Home.html
	 * You can also use the getter inside other methods of this class to get at the spring bean.
	 */
	//public abstract GreetingService getGreetingService();
 
	/**
	 * This returns the hardcoded greeting in this class.
	 */
	public String getGreeting() {
		return greeting;
	}
}
