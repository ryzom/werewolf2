<?php
	// jimckf@jimckf.com - 2010

	class mysql {
		private $link;
		public $query_count = 0;

		function connect($server,$username,$password) {
			$this->link = mysql_connect($server,$username,$password,true);
			if (!$this->link) {
				return false;
			}
			return true;
		}

		function select_db($name) {
			return mysql_select_db($name,$this->link);
		}

		function escape($string) {
			return mysql_real_escape_string($string,$this->link);
		}

		function query($query) {
			$this->query_count++;
			return mysql_query($query,$this->link);
		}

		function num_rows($result) {
			return mysql_num_rows($result);
		}

		function insert_id() {
			return mysql_insert_id($this->link);
		}

		function fetch_object($result) {
			return mysql_fetch_object($result);
		}
	}
?>
