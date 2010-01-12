<h1>Register</h1>
<?php
	global $logged_in;
	if ($logged_in) {
		echo 'You are already registered.';
	} else {
		if (!isset($_POST['username']) || !isset($_POST['password']) || !isset($_POST['password2']) || $_POST['password'] != $_POST['password2']) {
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
						</tr>
						<tr>
							<td><b>Password again:</b></td>
							<td><input type="password" name="password2"></td>
						</tr>
						<tr>
							<td colspan="2"><input type="submit" value="Register"></td>
						</tr>
					</table>
				</form>
			';
		} else {
			global $topaz;
			if (!$topaz->ww_users->user_create($_POST['username'],$_POST['password'])) {
				echo 'Registration failed. The username might be taken. Please <a href="register">try again</a> in a few moments.';
			} else {
				$topaz->ww_users->user_login($_POST['username'],$_POST['password']);
				echo 'Registration complete. Maybe we should redirect you somewhere?';
			}
		}
	}
?>
