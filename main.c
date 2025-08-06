//
// Arquivos de Inclusão
//
#include "driverlib.h"
#include "device.h"
#include "board.h"
#include "scicomm.h"
#include "math.h"

//
// Comunicação CLA
//
#pragma DATA_SECTION(fVal,    "CpuToCla1MsgRAM");
#pragma DATA_SECTION(fResult, "Cla1ToCpuMsgRAM");
#pragma DATA_SECTION(debug_d, "Cla1ToCpuMsgRAM");

// Comunicação com a CPU através da RAM de mensagem
extern float fVal;
extern float fResult;
extern float debug_d;

//
// Definições de Constantes
//
#define F_PWM                   10000.0f        // Frequência de chaveamento (Hz)
#define T_PWM                   (1.0f / F_PWM)  // Período de chaveamento (s)
#define DT_SIM                  0.000005f       // Passo de simulação (5 µs)
#define N_STEPS_PER_CYCLE       (uint32_t)(T_PWM / DT_SIM) // Passos por ciclo PWM
#define Vref_dac                3.3f   // REFERENCIA PARA O DAC
#define ValmaxDAC               4095   // VALOR MAXIMO DAC
#define INV_DAC                 ((4095/300))
#define INV_ADC                 ((float)((1/300)*4095))
#define inv 0.0732600f //300.0f / 4095.0f
//#define VOLT2DAC              ValmaxDAC/Vref_dac
#define inv_amp                 0.09768009768// 400.0f/4095 /
#define tripzone                (400 * 4095)/300
// Parâmetros do Conversor BOOST
#define VIN                    48.0f           // Tensão de entrada (V)
#define L                      0.0004f          // Indutância (H)
#define C                      0.000047f        // Capacitância (F)
#define R_LOAD                 5.0f           // Carga resistiva (Ohm)

// Constantes auxiliares
#define INV_L                  (DT_SIM / L)
#define INV_C                  (DT_SIM / C)
#define INV_R_LOAD             (1.0f / R_LOAD)
#define TAM_BUFFER_DAC         50
#define TAM_BUFFER_ADC         50
#define BUFFER_SIZE            50             // Tamanho do buffer de resultados
//
// Variáveis Globais da Simulação
//
volatile uint32_t iL = 0;
volatile float32_t g_vout_sim = 0;          // Tensão de saída simulada
volatile float32_t vo = 0;
volatile float32_t g_il_sim = 0.0f;          // Corrente no indutor simulada
volatile uint32_t g_step_counter = 0;  // Contador de passos dentro do ciclo PWM
volatile bool g_switch_on = false;        // Estado da chave (true = ligada)
volatile bool g_new_step_ready = false;   // Flag para novo passo de simulação
volatile float g_duty_cycle = 0.5f;        // Razão cíclica (entre 0 e 1)
volatile uint16_t adc_buffer;        //[TAM_BUFFER_ADC];
volatile uint16_t dac_buffer = 0;
volatile float32_t results[BUFFER_SIZE]; // Vetor com valores simulados de tensão
volatile float32_t REF = 48.0f;
int idx = 0;                                   // Índice circular para o buffer
float flag = 0;
volatile bool g_doSimulation = false;
static uint16_t cnt_dac = 0;
#pragma DATA_SECTION(fVal,"CpuToCla1MsgRAM");
float fVal;
#pragma DATA_SECTION(fResult,"Cla1ToCpuMsgRAM");
float fResult;
#pragma DATA_SECTION(debug_d,"Cla1ToCpuMsgRAM");
float debug_d;

__interrupt void INT_overiL_1_ISR(void)
{

}

__interrupt void xint1_isr(void)
{
    //XINT1Count = 1; // Incrementa o contador de interrupções
    if (GPIO_readPin(122) && flag == 0)
    {
        g_switch_on = true;

        flag = 1;
    }
    else
    {
        flag = 0;
        g_switch_on = false;
    }
    // Reconhece a interrupção no PIE
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

//
// Interrupção do ADC
//
//__interrupt void INT_ADC0_1_ISR(void)
//{
////    static uint16_t cnt_adc = 0;
//    adc_buffer = (ADC_readResult(ADC0_RESULT_BASE, ADC0_SOC0));
//    fVal = (float)adc_buffer * INV_ADC;
//    //CLA_forceTasks(myCLA0_BASE, CLA_TASKFLAG_1);
//    ADC_clearInterruptStatus(ADC0_BASE, ADC_INT_NUMBER1);
//    Interrupt_clearACKGroup(INT_ADC0_1_INTERRUPT_ACK_GROUP);
//}
//
// Função principal
//
void main(void)
    {
    float32_t v_l, i_c;

    // Inicialização
    Device_init();

    Interrupt_initModule();
    Interrupt_initVectorTable();
    Interrupt_register(INT_XINT1, &xint1_isr);   // REGISTRA ISR
    Interrupt_enable(INT_XINT1);                 // HABILITA XINT1 no PIE

    Board_init();

//    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    // Habilita interrupções globais
    EINT;
    ERTM;

    //
    // Loop principal da simulação
    //
    while (1)
    {
        if (g_new_step_ready)
        {
            fVal = (REF - (ADC_readResult(ADC0_RESULT_BASE, ADC0_SOC0)) * inv);//ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER0);
            CLA_forceTasks(CLA1_BASE, CLA_TASKFLAG_1);

            g_new_step_ready = false;
            // Cálculo da tensão no indutor
            v_l = g_switch_on ? VIN : (VIN - g_vout_sim);

            i_c = g_switch_on ?(-g_vout_sim * INV_R_LOAD) : (g_il_sim - (g_vout_sim * INV_R_LOAD));

            // Integração (Euler)
            g_il_sim += INV_L * v_l;
            g_vout_sim += INV_C * i_c;
            vo = g_vout_sim ;

            dac_buffer = ((uint16_t) (vo * INV_DAC)) - 96;
            if(dac_buffer > 4095)
            {
                dac_buffer = ((uint16_t)(4095));
            }

            iL = ((g_il_sim * inv_amp));

            if(iL > 4095.0f)
            {
               iL = ((4095.0f));
            }
            DAC_setShadowValue(DAC0_BASE, ((dac_buffer))); // O DAC
            DAC_setShadowValue(Overcurrent_BASE, (((uint16_t)(iL)))); // O DAC
            results[idx] = g_vout_sim;//(ADC_readResult(ADC0_RESULT_BASE, ADC0_SOC0)) * inv; //* inv_amp;
            idx = (idx + 1) % BUFFER_SIZE;
            cnt_dac = (cnt_dac + 1) % TAM_BUFFER_DAC;

        }

    }
}
//
// Interrupção do Timer (gera novo passo de simulação HIL)
//
__interrupt void INT_myCPUTIMER1_ISR(void)
{
    g_new_step_ready = true;

    // Libera nova interrupção
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

