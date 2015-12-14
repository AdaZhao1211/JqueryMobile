<?php

//echo "PHP";
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

require 'Slim/Slim.php';
\Slim\Slim::registerAutoloader();
$app = new \Slim\Slim();

//echo "Slim";

function dbConnect() {
  $dbHost = "localhost";
  //the name of database
  $dbName = "yz2029";
  $dbUser = "root";
  $dbPass = "root";

  $db = new PDO("mysql:host=$dbHost;dbname=$dbName", $dbUser, $dbPass);
  return $db;

}

echo "{";
echo '"api": {' . "\n";

  $app->post('/reading', 'insertReading');
  $app->get('/reading', 'insertReading');

  function insertReading() {
    $db = dbConnect();
    // the name of sheet, (the column name)
    $temp = $_GET['temp'];
    $humidity = $_GET['humidity'];
    $AQ = $_GET['AQ'];

    $insert = $db->prepare("INSERT INTO yun (temp,humidity,AQ) VALUES ($temp,$humidity,$AQ)");
    $insert->execute();
    echo '"status": "success"';
  }

  $app->get('/readings/', 'selectReadings');

  function selectReadings() {
    $db = dbConnect();
    $select = $db->query("SELECT * FROM yun");
    echo '"temp": ' . "[\n";
    $temps = '';
    while($row = $select->fetch()) {
      $temps .= $row['temp'] . ',';
    }
    $temps = rtrim($temps, ",");
    echo $temps;
    echo " ],\n";

    $select = $db->query("SELECT * FROM yun");
    echo '"humidity": ' . "[\n";
    $humiditys = '';
    while($row = $select->fetch()) {
      $humiditys .= $row['humidity'] . ',';
    }
    $humiditys = rtrim($humiditys, ",");
    echo $humiditys;
    echo " ],\n";

    $select = $db->query("SELECT * FROM yun");
    echo '"AQ": ' . "[\n";
    $AQs = '';
    while($row = $select->fetch()) {
      $AQs .= $row['AQ'] . ',';
    }
    $AQs = rtrim($AQs, ",");
    echo $AQs;
    echo " ]\n";
  }

  $app->run();
  echo "}\n";
  echo "}\n";


?>
