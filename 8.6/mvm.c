#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

void multi(int Count,float *Sum,float Vec[],float Data[],int row, int column, int *disp, int *sendcount){
  int rank, size, a, cc=0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  for(a=0;a<rank;a++){
      cc+=sendcount[a];
  }
    cc=cc/row;
  for(a=0;a<row;a++){
      Sum[a]=0;
  }
  int i=0,j=0,k=0, l=0;
  while(i<Count){
    for(j=0;j<row;j++){
       Sum[j] = Sum[j] + Data[k] * Vec[cc+i];
       k++;
    }
    i++;
  }
}

int main(int argc,char *argv[]){
  int rank,size,*sendcount,*displace,*reccount;
  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&size); 
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Status status;
  FILE *fp;
  char c;
  int i,j,k=0,count=0,row=0,column=0;
  float n=0,*sum,*rec_data,*data,*vec, *cdata;
  sendcount = (int*)calloc(sizeof(int),size);
  reccount  = (int*)calloc(sizeof(int),size);
  displace  = (int*)calloc(sizeof(int),size);
  if(rank==0){
    fp=fopen(argv[1],"r");
    while(fscanf(fp,"%f",&n)!=-1){ 
      c=fgetc(fp);
      if(c=='\n'){ row=row+1; }
      count++;
    }
    column=count/row; 
    printf("Row=%d column=%d proc=%d\n",row,column,size);
    float mat[row][column];
    fseek( fp, 0, SEEK_SET );
    data = (float*)calloc(sizeof(float),row*column);
    cdata = (float*)calloc(sizeof(float),row*column);
    vec  = (float*)calloc(sizeof(float),column);
    for(i=0;i<row;i++){
        for(j=0;j<column;j++){
          fscanf(fp,"%f",&mat[i][j]); 
          data[k] = mat[i][j];
          k++;
       }
    }
    
    fclose(fp);
    fp    =  fopen(argv[2],"r"); 
    count = 0;
    while(fscanf(fp,"%f",&n)!=-1){  count++; }
    printf("length of vector = %d\n",count);
    if(column!=count) { printf("Dimensions do not match.\nCode Terminated"); MPI_Abort(MPI_COMM_WORLD,0); }
    fseek( fp, 0, SEEK_SET );
    for(i=0;i<column;i++){
      fscanf(fp,"%f",&vec[i]); 
    }
    fclose(fp);
    count=0;
    while(1){
      for(i=0;i<size;i++){
        sendcount[i] = sendcount[i]+1;
        count++;
        if(count==column) break;  
      }
      if(count==column)  break;
    }
    for(i=1;i<size;i++){
      displace[i] = displace[i-1] + sendcount[i-1]*row;
      sendcount[i-1] = sendcount[i-1] * row;
    }
    sendcount[size-1] = sendcount[size-1] * row;
    for(i=0;i<size;i++)
      printf("sendcout=%d disp=%d\n",sendcount[i],displace[i]);
  }
    k=0;
    for(i=0;i<column;i++){
        for(j=0;j<row;j++){
            cdata[k]=data[i+j*column];
            k++;   
        }
    }   

  MPI_Bcast(&row,1,MPI_INT,0,MPI_COMM_WORLD);
  MPI_Bcast(&column,1,MPI_INT,0,MPI_COMM_WORLD);
  if(rank!=0) {
    vec = (float *)malloc(sizeof(float) * column);
  }
  MPI_Bcast(vec,column,MPI_FLOAT,0,MPI_COMM_WORLD);
  MPI_Bcast(sendcount,size,MPI_INT,0,MPI_COMM_WORLD);
  MPI_Bcast(displace,size,MPI_INT,0,MPI_COMM_WORLD);  
  rec_data=(float*)calloc(sizeof(float),sendcount[rank]);
  MPI_Scatterv(cdata,sendcount,displace,MPI_FLOAT,rec_data,sendcount[rank],MPI_FLOAT,0,MPI_COMM_WORLD);
  count=sendcount[rank]/row;
  sum=(float*)calloc(sizeof(float),row);
  multi(count,sum,vec,rec_data,row, column, displace, sendcount);
  float *result=(float *)calloc(sizeof(float),row*size);
  int disp[size];
  disp[0]=0;
  reccount[0]=row;  
  for(i=1;i<size;i++){
    disp[i] = disp[i-1] + row; 
    reccount[i]=row;
  }
  MPI_Gatherv(sum,row,MPI_FLOAT,result,reccount,disp,MPI_FLOAT,0,MPI_COMM_WORLD);
  if(rank==0)
  {
    printf("\nMatrix Vector Multiplication is:\n");
    for(j=0;j<row;j++){
        for(i=1;i<size;i++){
            result[j]+=result[i*row+j];
        }
    }
    
    for(i=0;i<row;i++){
        printf("%.3f\n",result[i]);
    }
  }
  free(vec);
  free(sum);
  free(sendcount);
  free(displace);
  free(reccount);
  free(rec_data); 
  MPI_Finalize();
  return 0;
}
