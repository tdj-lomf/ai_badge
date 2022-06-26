
void eyeCommand(int id) {
  const int TIME_M = 250; // [ms]
  switch (id) {
    case 1: moveEyeSync(0.0, 0.0, TIME_M); break;
    case 2: moveEyeSync(0.0, 90.0, TIME_M); break;
    case 3: moveEyeSync(0.0, -90.0, TIME_M); break;
    case 4: moveEyeSync(90.0, 0.0, TIME_M); break;
    case 5: moveEyeSync(-90.0, 0.0, TIME_M); break;
  }
}

void eyelidCommand(int id) {
  const int TIME_M = 300;
  double tmpU, tmpL;
  switch (id) {
    case 0:  // 全開
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      moveEyelidSync(0.0, 0.0, TIME_M);
      break;
    case 1: // 瞬き1回
      tmpU = getUState();
      tmpL = getLState();
      moveEyelidSync(100.0, 100.0, TIME_M);
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      moveEyelidSync(tmpU, tmpL, TIME_M); // 元の位置に戻る
      break;
    case 2: moveEyelidSync(50.0, 0.0, TIME_M); break; // ジト目Lv1
    case 3: moveEyelidSync(50.0, 50.0, TIME_M); break; // ジト目Lv2
    case 4: moveEyelidSync(0.0, 50.0, TIME_M); break; // ニヤケ
    case 5:  // 全閉
      moveEyelidSync(100.0, 100.0, TIME_M);
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, LOW);
      break;
  }

}