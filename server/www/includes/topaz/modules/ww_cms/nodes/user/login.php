<h1>Login</h1>
<?php
	global $logged_in;
	if ($logged_in) {
		echo 'You are already logged in.';
	} else {
		if (!isset($_POST['username']) || !isset($_POST['password'])) {
			echo '
				<form action="" method="post">
					<table>
						<tr>
							<td><b>Username:</b></td>
							<td><input type="text" name="username" value="' . $_POST['username'] . '"></td>
						</tr>
						<tr>
							<td><b>Password:</b></td>
							<td><input type="password" name="password"></td>
							<td><input type="submit" value="Login"></td>
						</tr>
					</table>
				</form>
				<a href="register">Don\'t have an account?</a> | <a href="forgot">Forgot your password?</a>
			';
		} else {
			global $topaz;
			if ($topaz->ww_users->user_login($_POST['username'],$_POST['password'])) {
				echo 'You are now logged in. Maybe we should redirect you somewhere?';
			} else {
				echo 'Invalid username and/or password. Please <a href="login">try again</a>.';
			}
		}
	}
?>
