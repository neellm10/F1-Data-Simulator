#include "declaration.h"

void* speed(){
	//printf("Calc Speed\n");
	stndval = (engSize / (float)carWeight) / 3;
	printf("Standard Value: %f\n", stndval);

	pthread_rwlock_wrlock(&rw_lock);
	for(y=0; y<lapSize; y++){
		float temp=0;
			for(i=0; i<segNo; i++){
				
				dist = segmentSize[i];
				float med=dist/2, bp1=med*0.285, bp2=med*1.710, speed=0;
		
				while(temp<dist){
					if(temp<=bp1){
						temp = temp + 12.7;
						speedArray[0] = 12.7 * 3.6 * stndval;
					}
					else if(temp>bp1 && temp<=med){
						temp = temp + 27.7;
						speedArray[1] = 27.7 * 3.6 * stndval;
					}
					else if(temp>med && temp<=bp2){
						temp = temp + 23.5;
						speedArray[2] = 23.5 * 3.6 * stndval;
					}
					else if(temp>bp2){
						temp = temp + 12.7;
						speedArray[3] = 12.7 * 3.6 * stndval;
					}
				}
				
				total += temp;
				//printf("Distance Travelled in Segment %d of Lap %d : %f\n", i+1, y+1, temp);
				//sleep(3);
			}
			printf("Distance Travelled in Lap %d : %f\n", y+1, temp);
			sleep(2);
	}
	pthread_rwlock_unlock(&rw_lock);
	printf("Total Distance Travelled: %fkm\n", total/1000);

	st = speedArray[0];
		for(int x = 1; x<4; x++){
			if(st<speedArray[x])
				st = speedArray[x];
		}

	printf("Peak Speed in the given Race: %fkmph\n", st);
	sleep(3);
}

void* tyre(){
	//printf("Tyre Entered!");
	if(tyreType==1)
		tyreCap = 25000;
	else if(tyreType==2)
		tyreCap = 35000;
	else if(tyreType==3)
		tyreCap = 50000;
	
	int temp = tyreCap;
	//printf("%d", tyreCap);

	//for(int i=0; i<segNo; i++){
		//sleep(3.25);
		pthread_rwlock_rdlock(&rw_lock);
		temp = temp - total;
		pthread_rwlock_unlock(&rw_lock);
		tyreAge = (temp*100)/(float)tyreCap;

		if(tyreAge<10)
			flag=2;
		if(tyreAge<2)
			flag=4;
	//}
	//while(1);
	//printf("Tyre Exited!");
	//printf("Tyre: %f %% is the tyre age.\n", tyreAge);
}	

void* fuel(){
	//printf("Fuel Entered!");
	mileage = engSize / (float)1500;

	//for(int i=0; i<segNo; i++){
	//	sleep(3.25);
		pthread_rwlock_rdlock(&rw_lock);
		tankCapacity = tankCapacity - ((total/10000)/(float)mileage);
		pthread_rwlock_unlock(&rw_lock);
		if(tankCapacity<2)
			flag=1;
		if(tankCapacity<0.25000)
			flag=3;
	//}
	//while(1);
	//printf("Fuel Exited!");
	//printf("Remaining Tank Capacity %f Litres\n", tankCapacity);
}

void status(){
	(*fuel)();
	(*tyre)();

	if(flag==0){
		printf("\n%s is on %d Segment of Lap %d", carName, i, y);
		printf(" With Fuel Tank Capacity of %f Liters and Tyre Age %f%%\n", tankCapacity, tyreAge);
	}

	else if (flag==1){
		printf("\n%s is on %d Segment of Lap %d", carName, i, y);
		printf(" With Fuel Tank Capacity of %f Liters and Tyre Age %f%%\n", tankCapacity, tyreAge);
		printf("Car running low on fuel! Please Refill!\n");
	}

	else if (flag==2){
		printf("\n%s is on %d Segment of Lap %d", carName, i, y);
		printf(" With Fuel Tank Capacity of %f Liters and Tyre Age %f%%\n", tankCapacity, tyreAge);
		printf("Tyres used too much! Change 'em\n");
	}

	else if (flag==3){
		printf("You should have refilled.!");
		exit(0);
	}

	else if (flag==4){
		printf("You should have changed the tyres.!");
		exit(0);
	}
}

void input(){
	printf("Enter information: ");
    printf("\nCar Name: ");
    fgets(carName, 20, stdin);
    carName[strlen(carName)-1] = '\0';
    printf("\nEngine Size (in terms of CC): ");
    scanf("%d", &engSize);
    printf("\nWeight of car: ");
    scanf("%d", &carWeight);
    printf("\n1. Soft");
    printf("\n2. Medium");
    printf("\n3. Hard");
    printf("\nEnter Tyre Type: ");
    scanf("%d", &tyreType);
    
    printf("\nEnter the Track File name: ");
    scanf("%s", trackFile);
    printf("\nNumber of Laps: ");
    scanf("%d", &lapSize);
}

void fileReading(){
	inputFile = fopen(trackFile, "r");
    if (inputFile == NULL)
        printf("\nCan't open %s for reading.", trackFile);

    else{
        int i = 0;
        while(1){
            int temp;
            temp = fscanf(inputFile, "%d", &segmentSize[i]);
            if(temp == EOF)
                break;
            else{
               // printf("Segment No.:%d, Length: %d\n", i+1, segmentSize[i]);
                i++;
            }
        }
        segNo = i;
}
}

void threadCreationDeletion(){
	void* (*signalHandler[])() = {speed, fuel, tyre};
	pthread_rwlock_init(&rw_lock, NULL);

	pthread_attr_t attr;
	struct sched_param param;

	pthread_attr_init (&attr);
	pthread_attr_getschedparam (&attr, &param);
	(param.sched_priority)++;
	pthread_attr_setschedparam (&attr, &param);

/*for(int i=0; i<3; i++){
		int err = pthread_create (&t[i], NULL, signalHandler[i], NULL);
            if (err != 0)
            	printf("cant create thread: %s\n", strerror(err));
	}*/
	pthread_create (&t[0], &attr, signalHandler[0], NULL);
	pthread_create (&t[1], NULL, signalHandler[1], NULL);
	pthread_create (&t[2], NULL, signalHandler[2], NULL);

	pthread_join(t[0], NULL);
	pthread_join(t[1], NULL);
	pthread_join(t[2], NULL);

	pthread_rwlock_destroy(&rw_lock);
}