<!DOCTYPE html>
<html>
<head>
<title>バッテリー異常リセット</title>
<meta charset="utf-8">
<link rel="stylesheet" type="text/css" href="Menu.css">
</head>
<body>
<div>
  <h1 style ="text-align:center;background-color:#111111; color:white;box-shadow: 0px 10px 8px rgba(2,2,0,0.5);">やまおくのほそみち管理ページ </h4>
  <!--text-align:center--場所（センター）__background-color:#111111--背景色(黒)__color:white--文字の色__box-shadow (横への影)px (縦方向への影)px (影のぼかし具合)px rgba(色)-->
</div>
<dev id="center">
  <div id="menu">
    <div id="left">
    <?php include ('Menubar.html'); ?>
  </div>
<body>
  <?php
    $place=$_GET['place'] ;
    $id1= substr($place,1,1);
    $id2=substr($place,2,1);
    $placenumber=(hexdec($id1))*2+(hexdec($id2)+1);                                               //placeのIDをナンバーに変える
   ?>
<h1 style="color:black">灯籠ID:<?php echo "$placenumber" ;?>のバッテリー警告消去</h1>
<form action="batteryclear.php?place=<?php echo $place ?>" method="post" style="color:black">
  本当に削除しますか？
  削除した場合次にバッテリー警告が出るまで表示されません。<br>
  削除する場合は削除ボタンを押してください。<br>
  <input type="submit" value="削除" >
<br>
<br>
</form>
<script src="js/jquery-1.11.1.min.js"></script>
<script>
function connecttext( textid, ischecked ) {
   if( ischecked == true ) {
      // チェックが入っていたら有効化
      document.getElementById(textid).disabled = false;
   }
   else {
      // チェックが入っていなかったら無効化
      document.getElementById(textid).disabled = true;
   }
}
</script>
</body>
</html>
