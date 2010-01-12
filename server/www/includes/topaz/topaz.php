<?php
	// Jim C. K. Flaten - 2010

	class Topaz {
		function module_load($module) {
			$module_file = realpath(dirname(__FILE__) . '/modules/' . $module . '.php');
			if (!file_exists($module_file)) {
				return false;
			} else {
				require_once($module_file);
				$this->$module = new $module;
				$this->$module->parent = &$this;
				return true;
			}
		}
	}
?>
