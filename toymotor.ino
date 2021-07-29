 //不带遥控功能的儿童电动童车改线性油门
//使用Arduino模拟端口接收霍尔油门踏板信号
//再由数字端口输出PWM信号控制有刷电调驱动双路直流电机
//ardunio nano,火凤凰480A,3S锂电池

int escPin = 9; //舵机接口
int throPadPin = A0;//踏板电位器模拟输入口
int throAdjPin = A2;//油门调整旋钮电位器输入口
int switchPin0 = 6; //前后开关
int switchPin1 = 4;//前后开关

int pulsewidth;//定义脉宽变量

int finalOutput;//计算后最终输出值
int myangle;

int throPadInput;//踏板最终输出
int throAdjInput;//油门调整输入

int throPadMin = 183;//踏板最小
int throPadMax = 863;//踏板最大

int expectMax = 390;//最大油门时踏板电位器值
int expectMaxtemp = 390;//最大油门时踏板电位器值
float expectMaxFloat;;//最大油门时踏板电位器值float

int forwardOutVal = 2480;
int reverseOutVal = 500;

int offsetMin;//期望输入大小偏移
int offsetMax;//实际输入大小偏移
float offset;
float adjustVal;//电位器调整量

int status = 0;//-1后退 0空 1前进





/**************/
int throAdjMax = 1023;
int throAdjmin = 0;
float throLimit = 0;//经过偏移后的百分比值乘以expectMax=390不能小于throPadMin=183
float throAdjOffset; 
int throFinalOffset;//经过加上throLimit偏移的计算值
/**************/

//servopulse函数部分(此函数意思:也就是說每次都是0.5ms高電平 1.98ms低電平 然後再0.52ms低電平 17ms延時也是低電平)
void servopulse(int escPin, int finalVal, int outVal) {
  myangle = map(finalVal, throPadMin, throPadMax, 1490, outVal);//370
  digitalWrite(escPin, HIGH); //将舵机接口电平至高
  delayMicroseconds(myangle);//延时脉宽值的微秒数
  digitalWrite(escPin, LOW); //将舵机接口电平至低
}


//开关状态
void getSwitchStat() {
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
  Serial.println("hello world,qq:416800698" ) ;

  throAdjInput = analogRead(throAdjPin);//读取油门调整电位器信号 只在启动一刻处理
  setupAdjOffset(throAdjInput);
  adjustVal = (float)throFinalOffset/1023;//百分化
  expectMaxFloat = (int)(expectMax*(float)adjustVal);
  expectMax = round(expectMaxFloat);//<--------------------------此处若放在循环中会反复计算 
  offsetMin = expectMax - throPadMin;
  offsetMax = throPadMax - throPadMin;
  offset = (float)offsetMin / offsetMax;
}


/*******************/


void setupAdjOffset(int throAdjInput){
  throLimit = ((float)throPadMin/expectMax)*throAdjMax;
  throAdjOffset = (throAdjMax-throLimit)/throAdjMax;
  throFinalOffset = throAdjMax -(throAdjMax-throAdjInput)*throAdjOffset;
  Serial.print("throFinalOffset =  ");
  Serial.println(throFinalOffset);
}
/*******************/


void loop()
{
  throPadInput = analogRead(throPadPin);//读取油门踏板信号
  throAdjInput = analogRead(throAdjPin);//读取油门调整电位器信号
  Serial.print("throPadInput = ");
  Serial.print(throPadInput, DEC);
  
  ////Serial.print(",throAdjInput = ");
  ////Serial.print(throAdjInput, DEC);
  
  Serial.print(",adjustVal = ");
  Serial.print(adjustVal, 3);
  
  ////Serial.print(",expectMaxFloat = ");
  ////Serial.print(expectMaxFloat, DEC);
  
  Serial.print(",expectMax = ");
  Serial.print(expectMax, DEC);
  
  finalOutput = expectMax - ((throPadMax - throPadInput) * offset);
  Serial.print(",finalOutput = ");
  Serial.print(finalOutput);
  
  Serial.print(",switch =  ");
  Serial.println(status);
  
  getSwitchStat();//获取开关状态
  if (status == 1) {
    servopulse(escPin, finalOutput, forwardOutVal); //引用脉冲函数
  }
  if (status == -1) {
    servopulse(escPin, finalOutput+5, reverseOutVal); //引用脉冲函数
  }
  if (status == 0) {
    //无操作
  }
}
