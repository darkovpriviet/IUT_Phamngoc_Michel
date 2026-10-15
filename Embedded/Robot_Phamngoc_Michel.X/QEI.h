#ifndef QEI_H
#define	QEI_H

#define POSITION_DATA 0x0061
#define FREQ_ECH_QEI  250.0
void InitQEI1(void);
void InitQEI2(void);
void QEIUpdateData(void);
void SendPositionData(void);

#endif	/* QEI_H */

