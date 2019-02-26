#include "pid.h"

//0、PID初始化函数，给各参数赋值
//参数(4个)：Kp，Ki，Kd，处理的PID结构体的地址
void pid_init(float Kp, float Ki, float Kd, PID_TypeDef* PID)
{
	PID->Kp = Kp;
	PID->Ki = Ki;
	PID->Kd = Kd;
}

//1、位置PID
//参数(3个)：当前位置，目标位置，处理的PID结构体的地址
int pid(int present, u16 target, PID_TypeDef* PID)
{
	PID->error = target-present;	//本次误差 = 目标值 - 实际值
	
	PID->p_out = PID->Kp * PID->error;//比例
	PID->i_out += PID->Ki * PID->error;//积分
	PID->d_out = PID->Kd * (PID->error - PID->last_error);//微分
	
	PID->output = PID->p_out + PID->i_out + PID->d_out;//输出
	
	PID->last_error = PID->error;//上次误差 = 本次误差
	
	return PID->output;
}


//2、改进版位置PID(对微分项进行改善，考虑历史信息，降噪)
//参数(3个)：当前位置，目标位置，处理的PID结构体的地址
int better_pid(int present, u16 target, PID_TypeDef* PID)
{
	PID->error = target-present;	//本次误差 = 目标值 - 实际值
	
	PID->p_out = PID->Kp * PID->error;//比例
	PID->i_out += PID->Ki * PID->error;//积分
	PID->d_out = PID->Kd * 1/16 * (PID->error + 3*PID->last_error + 2*PID->last_error2 -2*PID->last_error3 - 3*PID->last_error4 - PID->last_error5);//微分
	
	PID->output = PID->p_out + PID->i_out + PID->d_out;//输出
	
	PID->last_error5 = PID->last_error4;//上次误差 = 本次误差
	PID->last_error4 = PID->last_error3;//上次误差 = 本次误差
	PID->last_error3 = PID->last_error2;//上次误差 = 本次误差
	PID->last_error2 = PID->last_error;//上次误差 = 本次误差
	PID->last_error = PID->error;//上次误差 = 本次误差
	
	return PID->output;
}


//3、增量式PID(目前效果不好)
//参数(3个)：当前位置，目标位置，处理的PID结构体的地址
int incre_pid(int present, u16 target, PID_TypeDef* PID)
{
	PID->error = target-present;	//本次误差 = 目标值 - 实际值
	
	PID->p_out = PID->Kp * (PID->error - PID->last_error);//比例
	PID->i_out += PID->Ki * PID->error;//积分
	PID->d_out = PID->Kd * (PID->error - 2*PID->last_error + PID->last_error2);//微分
	
	PID->output += PID->p_out + PID->i_out + PID->d_out;//输出
	
	PID->last_error2 = PID->last_error;
	PID->last_error = PID->error;//上次误差 = 本次误差
	
	return PID->output;
}
