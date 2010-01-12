<?php
	$phpself = basename(__FILE__);
	$_SERVER['PHP_SELF'] = substr($_SERVER['PHP_SELF'],0,strpos($_SERVER['PHP_SELF'],$phpself)) . $phpself;

	require_once('includes/topaz/topaz.php');

	$topaz = new Topaz;

	$topaz->module_load('mysql');
	$topaz->module_load('ww_users');
	$topaz->module_load('ww_cms');

	$topaz->mysql->connect('localhost','NeL','Secret!');
	$topaz->mysql->select_db('NeL');

	$logged_in = $topaz->ww_users->session_start();
?>
<!DOCTYPE HTML>
<html>
	<head>
		<meta http-equiv="content-type" content="text/html;charset=utf-8">
		<title>Werewolf</title>
		<link rel="icon" href="<?php echo dirname($_SERVER['PHP_SELF']); ?>/style/favicon.ico" type="image/x-icon">
		<link rel="shortcut icon" href="<?php echo dirname($_SERVER['PHP_SELF']); ?>/style/favicon.ico" type="image/x-icon">
		<link rel="stylesheet" type="text/css" href="<?php echo dirname($_SERVER['PHP_SELF']); ?>/style/style.css">
	</head>
	<body>
		<div id="center">
			<div id="header">
				<h1><a href="<?php echo dirname($_SERVER['PHP_SELF']) . '/'; ?>"><img src="<?php echo dirname($_SERVER['PHP_SELF']); ?>/style/logo.png"></a></h1>
			</div>
			<div id="menu">
				<h2>Menu</h2>
				<?php
					$topaz->ww_cms->menu_print($_GET['q']);
				?>
			</div>
			<div id="content">
				<?php
					echo $_SESSION['username'] . '<hr>';

					$page = $topaz->ww_cms->node_load($_GET['q']);
					if ($page == false) {
						echo '<h1>Error</h1>Not found.';
					} else {
						echo $page;
					}

					echo '<br><br><br><br><hr><pre>' . $topaz->mysql->query_count . ' queries</pre>';
				?>
			</div>
		</div>
	</body>
</html>
