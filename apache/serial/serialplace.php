<?php
echo "\n---------------- P H P -------------------\n";
if (isset($argv[1])==False) {
  echo "Not Found Zigbee Data\n";
  echo "-----------------Finish-------------------\n\n";
  exit;
}
echo "PHP取得データ:",$argv[1],"___",$argv[2],"\n";
$tagid = $argv[1];                                                                                                               //tagの場所ID2文字を読み込む
$tagplace= $argv[2];
$mysqli =new mysqli('localhost','root','Keyu3102','test2');
if($mysqli->connect_errno){
    echo $mysqli->connect_errno.";".$mysqli->connect_errnor;
}                                                                                                                                    //SQLへの接続
  mysqli_set_charset($mysqli,'utf8');

  echo "tagPlace:",$tagplace,"\n";
  echo "tagID:",$tagid,"\n";
  if ($tagid=="7f0"){
    $sql="UPDATE `ne` SET `battery`=\"1\" WHERE place=\"$tagplace\"";      //先頭が０の場合""で囲ってやらないとSQLで消される
    echo "#####バッテリーレベル低下#####\n";
  }else{
    $now = date('Y-m-d H:i:s');                                  //現在時刻を保存
    $sql="UPDATE `relate` SET `place`=\"$tagplace\", `time`=\"$now\" WHERE tagid=\"$tagid\"";      //先頭が０の場合""で囲ってやらないとSQLで消される
    echo "MySQL実行コマンド:",$sql,"\n";                                                                                                              //SQＬ文を確認しましょう
  }

  $res = mysqli_query($mysqli,$sql);                                                                               //ここでSQL実行
  $mysqli->close();


  //データーベースのログ側にデータを蓄積するためにSQL文を追記しました

    $mysqlilog =new mysqli('localhost','root','Keyu3102','test2');
    $sqllog ="INSERT INTO log(`tagid`,`place`) VALUES ('$tagid','$tagplace')";                          //ログ専用テーブル
    $reslog = mysqli_query($mysqlilog,$sqllog);
    $mysqlilog->close();

  echo "---------------PHP Success----------------\n\n";
  ?>
