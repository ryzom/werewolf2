package client.gui;


public class SWTThreadAlreadyInstanciatedException extends Exception {
  public SWTThreadAlreadyInstanciatedException() {
    super("The SWT Thread has already been instanciated");
  }
}
