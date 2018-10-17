<?php
$mysqli =new mysqli('localhost','root','Keyu3102','test2');
if($mysqli->connect_errno){
    echo $mysqli->connect_errno.";".$mysqli->connect_errnor;
}

mysqli_set_charset($mysqli,'utf8');
$tag='05A532';
//$tag = $_POST['taginfo'];         //ここでタグの6文字をまとめて受信、$tagに入れる
$tagplace = substr($tag, 0,3);  //場所に関する情報のうちの先頭一文字
$tagid = substr($tag, -3,3);       //タグIDの3文字
echo "tagplace:",$tagplace;
echo "<br>";
echo "tagid:",$tagid;
echo "<br>";
$sql="INSERT INTO log (tagid,place) VALUES ('$tagid','$tagplace')";                           //SQLのINSERTを配列に
$res = mysqli_query($mysqli,$sql);                                                                                //SQL実行と結果をダンプに
var_dump($res);
$mysqli->close();
?>
