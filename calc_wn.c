#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "GFhost.h"
#include "GFvec3.h"

Vec3 maxValue(Vec3 nums[], int n) {
    Vec3 max_value; /* 最大値 */
    int i;

    /* nums[0]を最大値と仮定する */
    max_value = nums[0];

    for (i = 0; i < n; i++) {
        if (nums[i].x > max_value.x) {
            max_value.x = nums[i].x;
        }
        if (nums[i].y > max_value.y) {
            max_value.y = nums[i].y;
        }
        if (nums[i].z > max_value.z) {
            max_value.z = nums[i].z;
        }
    }

    return max_value;
}

Vec3 minValue(Vec3 nums[], int n) {
    Vec3 min_value; /* 最小値 */
    int i;

    /* nums[0]を最小値と仮定する */
    min_value = nums[0];

    for (i = 0; i < n; i++) {
        if (nums[i].x < min_value.x) {
            min_value.x = nums[i].x;
        }
        if (nums[i].y < min_value.y) {
            min_value.y = nums[i].y;
        }
        if (nums[i].z < min_value.z) {
            min_value.z = nums[i].z;
        }
    }

    return min_value;
}

double func(double x){
    return 2*x-1;
}

int main(){
    Vec3 *points,*poly;
    PLIST *v_list,*p_list;

    int n_face,n_face_points,n_points;
    int tmp;

    //ポリゴンの読み込み
    FILE *fp;
    char fname[] = "sphere13500.txt";
    fp = fopen(fname,"r");
    if(fp == NULL) {
		printf("%s file not open!\n", fname);
		return -1;
	}

    //点の座標
    fscanf(fp,"%d",&n_face_points);
    poly = (Vec3 *)malloc(n_face_points * sizeof(Vec3));
    for(int i = 0 ; i < n_face_points ; i++){
        fscanf(fp,"%d%lf%lf%lf",&tmp,&poly[i].x,&poly[i].y,&poly[i].z);
    }

    //ポリゴン
    fscanf(fp,"%d",&n_face);
    p_list = (PLIST *)malloc(n_face * sizeof(PLIST));
    int a,b,c;
    for(int i = 0; i < n_face ; i++){
        fscanf(fp,"%d%d%d%d",&tmp,&a,&b,&c);
        p_list[i].v[0] = a - 1;   //配列の番号にあわせる　
        p_list[i].v[1] = b - 1;
        p_list[i].v[2] = c - 1;
    }
    fclose(fp);
    //データ読み込み終了


    //最大・最小
    Vec3 max_value,min_value;
    max_value = maxValue(poly,n_face_points);
    min_value = minValue(poly,n_face_points);
    //printf("%f %f %f \n",max_value.x,max_value.y,max_value.z);
    //printf("%f %f %f \n",min_value.x,min_value.y,min_value.z);

    //点の生成
    long idum = -1;
    double ran2(long *);
    scanf("%d",&n_points);     //点の個数
    points = (Vec3 *)malloc(n_points * sizeof(Vec3));

    for(int i=0 ;i < n_points; i++){                         //乱数生成
        points[i].x=func(ran2(&idum));
        points[i].y=func(ran2(&idum));
        points[i].z=func(ran2(&idum));
    }

    //WNの計算
    int count_1=0;
    int count_2=0;
    int *flag;
    flag = (int *)malloc(n_points * sizeof(int));

    long cpu_time;
    double sec;
    for(int k = 0 ; k < 10 ; k++){
        clock_t c_start,c_end;
        c_start = clock();
    for(int j = 0 ; j < n_points ; j++){
        double winding_number = 0.0;
        Vec3 A,B,C;
        //#pragma omp parallel for reduction(+:winding_number)
        for ( int i = 0; i < n_face; i++){
            A = poly[p_list[i].v[0]];
            B = poly[p_list[i].v[1]];
            C = poly[p_list[i].v[2]];

            A = Vsub(A,points[j]);
            B = Vsub(B,points[j]);
            C = Vsub(C,points[j]);

            double nA = Vnorm(A);
            double nB = Vnorm(B);
            double nC = Vnorm(C);
            winding_number += atan2(Vdot(A,Vcross(B,C)) , (nA*nB*nC + Vdot(A,B)*nC +Vdot(B,C)*nA + Vdot(C,A)*nB ));
        }
    
        double ep = 1.0e-13;
        if (winding_number > 2 * M_PI - ep){
            ++count_1;
            flag[j] = 1;
        }
        else flag[j] = 0;
    }
    c_end = clock();
    cpu_time = c_end - c_start;
    sec = (double)cpu_time / CLOCKS_PER_SEC;

    //内側の点の抽出
    Vec3 *points_in;
    points_in = (Vec3 *)malloc(count_1 * sizeof(Vec3));
    int cnt=0;
    for(int i = 0; i < n_points ; i++){
        if(flag[i] == 1){
            points_in[cnt] = points[i];
            //printf("%lf %lf \n", points_in[cnt].x,points[i].x);
            cnt++;
        }  
    }
    double v,v1;
    v = (double)count_1 / (double)n_points * (double)8;
    v1 = (double)4/3 * M_PI;
    printf("%lf %lf %lfs\n",v,v1,sec);
    count_1 = 0;

    }
    free(poly);
    free(p_list);
    free(points);
    //free(flag);
    //free(points_in);

    return 0;    
}