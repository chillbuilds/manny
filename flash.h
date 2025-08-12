for(int n = 0; n < 5; n++){
  for(int i = 0; i < 64; i++){
    analogWrite(led, i);
    delay(6);
  }
  for(int i = 64; i > 0; i--){
    analogWrite(led, i);
    delay(6);
  }
}