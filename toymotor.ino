int escPin = 9; //舵机接口
int throPin = A0;//踏板模拟输入口
int switchPin0 = 6; //开关
int switchPin1 = 4;//开关

int pulsewidth;//定义脉宽变量

int finalVal;
int myangle1;

int input;//踏板输入
int throPadMin = 183;//踏板最小
int throPadMax = 863;//踏板最大

int expectMax = 390;//最大油门时电位器值

int forwardOutVal = 2480;
int reverseOutVal = 500;

int offsetMin;//期望输入大小偏移
int offsetMax;//实际输入大小偏移
float offset;
/***********************/
int status = 0;//-1后退 0空 1前进
/***********************/
int inited = 1;

//下面是servopulse函数部分(此函数意思:也就是說每次都是0.5ms高電平 1.98ms低電平 然後再0.52ms低電平 17ms延時也是低電平)
void servopulse(int escPin, int finalVal, int outVal) {
  myangle1 = map(finalVal, throPadMin, throPadMax, 1490, outVal);//370
  digitalWrite(escPin, HIGH); //将舵机接口电平至高
  delayMicroseconds(myangle1);//延时脉宽值的微秒数
  digitalWrite(escPin, LOW); //将舵机接口电平至低
}


//开关状态
void getstat() {
  int val0 = digitalRead(switchPin0);
  int finalVal = digitalRead(switchPin1);
  if (val0 != 0 && finalVal == 0) {
    //Serial.println("REVERSE");
    status = -1;
  }
  if (val0 == 0 && finalVal != 0) {
    //Serial.println("FORWARD");
    status = 1;
  }
  if (val0 == 1 && finalVal == 1) {
    status = 0;
    //Serial.println("NEUTRAL");
  }

}

//servopulse函数部分到此结束
void setup()
{
  pinMode(switchPin0, INPUT_PULLUP);
  pinMode(switchPin1, INPUT_PULLUP);
  pinMode(escPin, OUTPUT); //设定舵机接口为输出接口
  Serial.begin(9600);
  Serial.println("hello world,QQ416800698" ) ;

  offsetMin = expectMax - throPadMin;
  offsetMax = throPadMax - throPadMin;
  offset = (float)offsetMin / offsetMax;

}

void loop()//将0到9的数转化为0到180角度，并让LED闪烁相应数的次数
{
  
  getstat();
  input = analogRead(throPin);
  Serial.print("now input is ");
  Serial.print(input, DEC);
  input = expectMax - ((throPadMax - input) * offset);
  Serial.print(",offet is ");
  Serial.print(input);
  Serial.print(",switch is  ");
  Serial.println(status);

  if (status == 1 && inited == 1) {
    servopulse(escPin, input, forwardOutVal); //引用脉冲函数
  }
  if (status == -1 && inited == 1) {
    servopulse(escPin, input+5, reverseOutVal); //引用脉冲函数
  }
  if (status == 0) {
    
  }
  
}
