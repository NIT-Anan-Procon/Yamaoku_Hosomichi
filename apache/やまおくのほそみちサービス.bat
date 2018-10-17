@echo off 
title やまおくのほそみちサービス起動
echo メール自動送信サービス＆USBZigbeeトングル受信起動サービスの実行
start python serial/serialread.c.pyc
start python AUTO-E-Mail/CheckloopRun.cpython-37.pyc