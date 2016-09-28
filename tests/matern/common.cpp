#include "petsc.h"
#include "common.h"
#include "assert.h"
#include <vector>

#define POLAR_FORM

void myPetscInit(int argc, char *argv[])
{
  PetscErrorCode ierr;
  ierr = PetscInitialize(&argc,&argv,0,0);CHKERRV(ierr);
}

void myPetscFinal()
{
  PetscErrorCode ierr;
  ierr = PetscFinalize(); CHKERRV(ierr);
}

double urand1(PetscRandom rctx)
{
    if (!rctx)
        return 2*((double)rand() / RAND_MAX)-1;
    double U;
    PetscRandomGetValueReal(rctx, &U);
    return U;
}


int urand(PetscRandom rctx, int N, double *U)
{
    int k;
    for (k=0;k<N;k++)
        U[k] = urand1(rctx);
    return 0;
}

int nrand(PetscRandom rctx, int N, double *G)
{
    int k=0;
    while(k<N)
    {
        // Tempory solution! Use Box-Muller
        // note rand/(rand_max+1) is U[0,1). 1-this is U(0,1] - avoid log(0)
#ifdef POLAR_FORM
        double u = urand1(rctx);
        double v = urand1(rctx);
        double s = u*u+v*v;
        if (s>1 || s==0)
            continue;
        double ls = sqrt(-2*log(s)/s);
        G[k++] = u*ls;
        if (k>=N) break;
        G[k++] = v*ls;
#else
        const double pi = M_PI;
        double U1 = 0.5*(1+urand1(rctx));        // Transform to U(0,1]
        double U2 = 0.5*(1+urand1(rctx));        // Transform to U(0,1]
        double f1 = sqrt(-2.0*log(U1));
        double f2 = 2*pi*U2;
        G[k++]   = f1*cos(f2);
        if (k>=N) break;
        G[k++] = f1*sin(f2);
#endif
    }
    return 0;
}

void legendre_quadrature(int *n, const double **points, const double **weights)
{
    /*
       Needs["NumericalDifferentialEquationAnalysis`"]
       For[n = 2, n <= 11, n++,
 d = GaussianQuadratureWeights[n, -1, 1, 37];
 P = d[[All, 1]];
 W = (1/2)*d[[All, 2]];
 Print[StringForm[
   "static const double qpt_``[]= ``;\nstatic const double w_``[]= \
``;", n, StringJoin[ToString[CForm[#], TraditionalForm], "L"] & /@ P, n,
   StringJoin[ToString[CForm[#], TraditionalForm], "L"] & /@ W]];]
Print["switch(*n){"]
For[n = 2, n <= 11, n++,
 Print[StringForm["case `1`: *points=qpt_`1`; *weights=w_`1`;break;",
   n]]]
Print["}"]
       */

    static const double qpt_2[]= {-0.57735026918962576450914878050195746L,0.57735026918962576450914878050195746L};
    static const double w_2[]= {0.5L,0.5L};
    static const double qpt_3[]= {-0.77459666924148337703585307995647992L,0L,0.77459666924148337703585307995647992L};
    static const double w_3[]= {0.277777777777777777777777777777777778L,0.444444444444444444444444444444444444L,0.277777777777777777777777777777777778L};
    static const double qpt_4[]= {-0.86113631159405257522394648889280951L,-0.33998104358485626480266575910324469L,0.33998104358485626480266575910324469L,0.8611363115940525752239464888928095L};
    static const double w_4[]= {0.173927422568726928686531974610999704L,0.326072577431273071313468025389000296L,0.326072577431273071313468025389000296L,0.173927422568726928686531974610999704L};
    static const double qpt_5[]= {-0.90617984593866399279762687829939297L,-0.5384693101056830910363144207002088L,0L,0.5384693101056830910363144207002088L,0.906179845938663992797626878299393L};
    static const double w_5[]= {0.118463442528094543757132020359958681L,0.239314335249683234020645757417819096L,0.284444444444444444444444444444444444L,0.239314335249683234020645757417819096L,0.118463442528094543757132020359958681L};
    static const double qpt_6[]= {-0.93246951420315202781230155449399461L,-0.6612093864662645136613995950199053L,-0.2386191860831969086305017216807119L,0.2386191860831969086305017216807119L,0.6612093864662645136613995950199053L,0.9324695142031520278123015544939946L};
    static const double w_6[]= {0.085662246189585172520148071086366447L,0.180380786524069303784916756918858056L,0.233956967286345523694935171994775497L,0.233956967286345523694935171994775497L,0.180380786524069303784916756918858056L,0.085662246189585172520148071086366447L};
    static const double qpt_7[]= {-0.9491079123427585245261896840478513L,-0.7415311855993944398638647732807884L,-0.4058451513773971669066064120769615L,0L,0.4058451513773971669066064120769615L,0.7415311855993944398638647732807884L,0.9491079123427585245261896840478513L};
    static const double w_7[]= {0.0647424830844348466353057163395410092L,0.139852695744638333950733885711889791L,0.190915025252559472475184887744487567L,0.208979591836734693877551020408163265L,0.190915025252559472475184887744487567L,0.139852695744638333950733885711889791L,0.0647424830844348466353057163395410092L};
    static const double qpt_8[]= {-0.960289856497536231683560868569473L,-0.7966664774136267395915539364758304L,-0.5255324099163289858177390491892463L,-0.183434642495649804939476142360184L,0.183434642495649804939476142360184L,0.5255324099163289858177390491892463L,0.7966664774136267395915539364758304L,0.960289856497536231683560868569473L};
    static const double w_8[]= {0.0506142681451881295762656771549810951L,0.111190517226687235272177997213120442L,0.156853322938943643668981100993300657L,0.181341891689180991482575224638597806L,0.181341891689180991482575224638597806L,0.156853322938943643668981100993300657L,0.111190517226687235272177997213120442L,0.0506142681451881295762656771549810951L};
    static const double qpt_9[]= {-0.9681602395076260898355762029036729L,-0.8360311073266357942994297880697349L,-0.6133714327005903973087020393414742L,-0.3242534234038089290385380146433366L,0L,0.3242534234038089290385380146433366L,0.6133714327005903973087020393414742L,0.8360311073266357942994297880697349L,0.9681602395076260898355762029036729L};
    static const double w_9[]= {0.0406371941807872059859460790552618253L,0.0903240803474287020292360156214564048L,0.130305348201467731159371434709316425L,0.156173538520001420034315203292221833L,0.165119677500629881582262534643487024L,0.156173538520001420034315203292221833L,0.130305348201467731159371434709316425L,0.0903240803474287020292360156214564048L,0.0406371941807872059859460790552618253L};
    static const double qpt_10[]= {-0.9739065285171717200779640120844521L,-0.865063366688984510732096688423493L,-0.6794095682990244062343273651148736L,-0.4333953941292471907992659431657842L,-0.14887433898163121088482600112972L,0.14887433898163121088482600112972L,0.4333953941292471907992659431657842L,0.6794095682990244062343273651148736L,0.865063366688984510732096688423493L,0.9739065285171717200779640120844521L};
    static const double w_10[]= {0.0333356721543440687967844049466658964L,0.074725674575290296572888169828848666L,0.109543181257991021997767467114081596L,0.134633359654998177545613460784734676L,0.147762112357376435086946497325669165L,0.147762112357376435086946497325669165L,0.134633359654998177545613460784734676L,0.109543181257991021997767467114081596L,0.074725674575290296572888169828848666L,0.0333356721543440687967844049466658964L};
    static const double qpt_11[]= {-0.9782286581460569928039380011228574L,-0.8870625997680952990751577693039273L,-0.7301520055740493240934162520311535L,-0.5190961292068118159257256694586096L,-0.2695431559523449723315319854008615L,0L,0.2695431559523449723315319854008615L,0.5190961292068118159257256694586096L,0.7301520055740493240934162520311535L,0.8870625997680952990751577693039273L,0.9782286581460569928039380011228574L};
    static const double w_11[]= {0.0278342835580868332413768602212742894L,0.0627901847324523123173471496119700501L,0.0931451054638671257130488207158279458L,0.11659688229599523995926185242158757L,0.131402272255123331090344434945254598L,0.136462543388950315357241764168171095L,0.131402272255123331090344434945254598L,0.11659688229599523995926185242158757L,0.0931451054638671257130488207158279458L,0.0627901847324523123173471496119700501L,0.0278342835580868332413768602212742894L};
    switch(*n){
        case 2: *points=qpt_2; *weights=w_2;break;
        case 3: *points=qpt_3; *weights=w_3;break;
        case 4: *points=qpt_4; *weights=w_4;break;
        case 5: *points=qpt_5; *weights=w_5;break;
        case 6: *points=qpt_6; *weights=w_6;break;
        case 7: *points=qpt_7; *weights=w_7;break;
        case 8: *points=qpt_8; *weights=w_8;break;
        case 9: *points=qpt_9; *weights=w_9;break;
        case 10: *points=qpt_10; *weights=w_10;break;
        case 11: *points=qpt_11; *weights=w_11;break;
        default:
                 printf("Unsupported number of quadrature points, %d\n", *n);
                 assert(0);
    }
}

void hermite_quadrature(int *n, const double **points, const double **weights)
{
    /*
       HermiteQuadratureWeights[n0_, digits_] :=
       Module[{n = n0, d = digits},
       xvec = x /. NSolve[HermiteH[n, x] == 0, x, digits];
       wvec = Gamma[n] * 2^n / s'[xvec] / HermiteH[n - 1, xvec];
       xvec = Sqrt[2]*xvec;
       Transpose[{xvec, wvec}]
       ]
       For[n = 2, n <= 11, n++, d = HermiteQuadratureWeights[n, 37];
       P = d[[All, 1]];
       W = d[[All, 2]];
       Print[StringForm[
       "static const double qpt_``[]= ``;\nstatic const double w_``[]= \
       ``;", n, StringJoin[ToString[CForm[#], TraditionalForm], "L"] & /@ P,
       n, StringJoin[ToString[CForm[#], TraditionalForm], "L"] & /@ W]];]
       Print["switch(*n){"]
       For[n = 2, n <= 11, n++,
       Print[StringForm["case `1`: *points=qpt_`1`; *weights=w_`1`;break;",
       n]]]
       Print["}"]
     */
    static const double qpt_2[]= {-1.L,1.L};
    static const double w_2[]= {0.5L,0.5L};
    static const double qpt_3[]= {-1.732050807568877293527446341505872367L,0L,1.732050807568877293527446341505872367L};
    static const double w_3[]= {0.166666666666666666666666666666666667L,0.6666666666666666L,0.166666666666666666666666666666666667L};
    static const double qpt_4[]= {-2.334414218338977239317512267210362194L,-0.7419637843027258576485135967263602248L,0.7419637843027258576485135967263602248L,2.334414218338977239317512267210362194L};
    static const double w_4[]= {0.0458758547680684918168929937745090507L,0.454124145231931508183107006225490949L,0.454124145231931508183107006225490949L,0.0458758547680684918168929937745090507L};
    static const double qpt_5[]= {-2.856970013872805654162304264006118334L,-1.355626179974265865830521290871584058L,0L,1.355626179974265865830521290871584058L,2.856970013872805654162304264006118334L};
    static const double w_5[]= {0.0112574113277206889333702151855760489L,0.222075922005612644399963118147757284L,0.5333333333333333L,0.222075922005612644399963118147757284L,0.0112574113277206889333702151855760489L};
    static const double qpt_6[]= {-3.324257433552118952361835462471466209L,-1.889175877753710675505667898580605543L,-0.6167065901925941521936860993989062692L,0.6167065901925941521936860993989062692L,1.889175877753710675505667898580605543L,3.324257433552118952361835462471466209L};
    static const double w_6[]= {0.0025557844020562464306062907438340055L,0.088615746041914527480855883005707386L,0.408828469556029226088537826250458608L,0.408828469556029226088537826250458608L,0.088615746041914527480855883005707386L,0.0025557844020562464306062907438340055L};
    static const double qpt_7[]= {-3.750439717725742256303922025712380804L,-2.366759410734541288618856468559933327L,-1.154405394739968127239597758837986485L,0L,1.154405394739968127239597758837986485L,2.366759410734541288618856468559933327L,3.750439717725742256303922025712380804L};
    static const double w_7[]= {0.00054826885597221779162157053280203388L,0.030757123967586497039645005716386711L,0.240123178605012713740161995179382683L,0.45714285714285713L,0.240123178605012713740161995179382683L,0.030757123967586497039645005716386711L,0.00054826885597221779162157053280203388L};
    static const double qpt_8[]= {-4.144547186125894332060197839166322836L,-2.802485861287541699113010806181706389L,-1.636519042435107999225446572966268903L,-0.5390798113513751080724619186939983832L,0.5390798113513751080724619186939983832L,1.636519042435107999225446572966268903L,2.802485861287541699113010806181706389L,4.144547186125894332060197839166322836L};
    static const double w_8[]= {0.0001126145383753677703938020168695757L,0.009635220120788267186919137719878456L,0.117239907661759015117137525961955125L,0.373012257679077349925549534301296843L,0.373012257679077349925549534301296843L,0.117239907661759015117137525961955125L,0.009635220120788267186919137719878456L,0.0001126145383753677703938020168695757L};
    static const double qpt_9[]= {-4.512745863399782667566678843166755007L,-3.205429002856469943365675902915643598L,-2.076847978677830106522156143744297617L,-1.023255663789132524828148225809651534L,0L,1.023255663789132524828148225809651534L,2.076847978677830106522156143744297617L,3.205429002856469943365675902915643598L,4.512745863399782667566678843166755007L};
    static const double w_9[]= {0.000022345844007746583648463990711772565L,0.0027891413212317686288134457516357257L,0.049916406765217874043341469382593738L,0.244097502894939436141022017700455589L,0.40634920634920635L,0.244097502894939436141022017700455589L,0.049916406765217874043341469382593738L,0.0027891413212317686288134457516357257L,0.000022345844007746583648463990711772565L};
    static const double qpt_10[]= {-4.859462828332312150155164946604391213L,-3.581823483551926922776236755457457443L,-2.484325841638954580876251183684908536L,-1.465989094391158183250664664164166916L,-0.4849357075154976530462334831049496222L,0.4849357075154976530462334831049496222L,1.465989094391158183250664664164166916L,2.484325841638954580876251183684908536L,3.581823483551926922776236755457457443L,4.859462828332312150155164946604391213L};
    static const double w_10[]= {4.3106526307182867322209547261998e-6L,0.0007580709343122176700696360365079834L,0.019111580500770285604738368762911155L,0.135483702980267735563431657727398946L,0.344642334932019042875028116518455716L,0.344642334932019042875028116518455716L,0.135483702980267735563431657727398946L,0.019111580500770285604738368762911155L,0.0007580709343122176700696360365079834L,4.3106526307182867322209547261998e-6L};
    static const double qpt_11[]= {-5.188001224374870948186664045389545972L,-3.936166607129976928685896121422531779L,-2.865123160643644997719684072543600398L,-1.876035020154845845341370139670945428L,-0.9288689973810639401441119995839235199L,0L,0.9288689973810639401441119995839235199L,1.876035020154845845341370139670945428L,2.865123160643644997719684072543600398L,3.936166607129976928685896121422531779L,5.188001224374870948186664045389545972L};
    static const double w_11[]= {8.121849790214914225812408309117695e-7L,0.0001956719302712233899294198996788214L,0.006720285235537278706350054717085242L,0.06613874607105782121720240438712003L,0.242240299873969951010391355051100288L,0.3694083694083694L,0.242240299873969951010391355051100288L,0.06613874607105782121720240438712003L,0.006720285235537278706350054717085242L,0.0001956719302712233899294198996788214L,8.121849790214914225812408309117695e-7L};
    switch(*n){
        case 2: *points=qpt_2; *weights=w_2;break;
        case 3: *points=qpt_3; *weights=w_3;break;
        case 4: *points=qpt_4; *weights=w_4;break;
        case 5: *points=qpt_5; *weights=w_5;break;
        case 6: *points=qpt_6; *weights=w_6;break;
        case 7: *points=qpt_7; *weights=w_7;break;
        case 8: *points=qpt_8; *weights=w_8;break;
        case 9: *points=qpt_9; *weights=w_9;break;
        case 10: *points=qpt_10; *weights=w_10;break;
        case 11: *points=qpt_11; *weights=w_11;break;

        default:
                 printf("Unsupported number of quadrature points, %d\n", *n);
                 assert(0);
    }
}


std::vector<ind_t> TDSet(ind_t d, uint32 count, unsigned int base){
    assert(d>=1);
    std::vector<ind_t> res;
    if (!count) return res;
    for (unsigned int k=0;k<d;k++){
        res.push_back(base);
    }
    std::vector<int> branch_point_prev;
    branch_point_prev.push_back(0);
    unsigned int cur_count = 1;   // Already added base
    unsigned int prev = 0;
    while (cur_count < count){
        std::vector<int> branch_point;
        for (unsigned int il=0;il<branch_point_prev.size();il++){
            for (unsigned int j=branch_point_prev[il];j<d;j++){
                branch_point.push_back(j);
                for (unsigned int k=0;k<d;k++){
                    res.push_back(res[(il+prev)*d+k] + (k==j));
                }
                cur_count++;
                if (cur_count >= count) break;
            }
            if (cur_count >= count) break;
        }
        prev += branch_point_prev.size();
        branch_point_prev = branch_point;
    }
    return res;
}

void GenTDSet(ind_t d, ind_t base, ind_t *td_set, uint32 count){
    std::vector<ind_t> ind_set = TDSet(d, count, base);
    assert(ind_set.size()/d==count);
    uint32 k=0;
    for (std::vector<ind_t>::iterator itr=ind_set.begin();itr!=ind_set.end();itr++){
        td_set[k++] = *itr;
    }
}

#include <iostream>

ind_t* TensorGrid(ind_t d, uint32 td,
                  ind_t base, const ind_t *m, ind_t *cur, ind_t i,
                  ind_t* tensor_grid, uint32* pCount){
    if (i >= d){
        assert(td == 0);
        for (ind_t k=0;k<d;k++)
            *(tensor_grid++) = base+cur[k];
        (*pCount)--;
        return tensor_grid;
    }
    ind_t max= m[i]-base; //std::min(static_cast<ind_t>(m[i]-base), static_cast<ind_t>(td));
    if (max > td) max = td;
    for (cur[i]=(i==d-1)?td:0;
         cur[i]<=max;
         cur[i]++){
        tensor_grid = TensorGrid(d, td-cur[i], base, m, cur,
                                 i+1, tensor_grid, pCount);
        if (!(*pCount)) break;
    }
    return tensor_grid;
}


void TensorGrid(ind_t d, ind_t base,
                const ind_t *m, ind_t* tensor_grid, uint32 count){
    // Returns a tensorized product of (1 ... m[0]) x (1 ... m[1]) ..
    // sorted by degree
    uint32 max_degree=0;
    for (ind_t i=0;i<d;i++) {
        assert(m[i] >= base);
        max_degree += m[i]-base+1;
    }
    ind_t cur[d];
    for (uint32 td=0;td<=max_degree;td++){
        tensor_grid=TensorGrid(d, td, base, m, cur, 0, tensor_grid, &count);
        if (!count) break;
    }
}
