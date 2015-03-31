%
% =====================================================================
%          Verze:  1.0
%      Vytvoreno:  01/02/2015 03:47:36 PM
%          Autor:  Tomáš Coufal
%          Login:  xcoufa09
%        Projekt:  ISS
% =====================================================================
%


% pokus cislo dve, tentokrat matlab
% python mi bohuzel dela problem v zaokrouhlovani (numpy, konverze na cele
% cisla) a medianovy filtr v PIL neumi pracovat s okraji stejne jako matlab,
% takze dostavam jine obrazky

% nacteni dat
i = imread('xcoufa09.bmp');
[dimX, dimY] = size(i);

% otevreni souboru pro zapis vysledku
results = fopen('reseni.txt', 'w');

% Zaostreni
focus = [ -0.5 -0.5 -0.5;
          -0.5    5 -0.5;
          -0.5 -0.5 -0.5 ];

step1 = imfilter(i,focus);
imwrite(step1, 'step1.bmp');

% Preklopeni podle svisle osy
step2 = flipdim(step1, 2);
imwrite(step2, 'step2.bmp');

% Medianovy filtr
step3 = medfilt2(step2, [5 5]);
imwrite(step3, 'step3.bmp');

% Rozostreni
blur = [ 1 1 1 1 1;
         1 3 3 3 1;
         1 3 9 3 1;
         1 3 3 3 1;
         1 1 1 1 1 ] / 49;

step4 = imfilter(step3,blur);
imwrite(step4, 'step4.bmp');

% Vypocet chyby
step5 = flipdim(step4, 2);
noise = 0;
data1 = double(i);
data2 = double(step5);

for (x = 1:dimX)
  for (y = 1:dimY)
    noise = noise + double(abs(data1(x,y) - data2(x,y)));
  end;
end;
noise = noise / (dimX * dimY);

fprintf(results, 'chyba=%f\n', noise);

% roztazeni histogramu
data = im2double(step4);
minVal = min(min(data));
maxVal = max(max(data));
step5 = imadjust(step4, [minVal, maxVal], [0, 1]);
imwrite(step5, 'step5.bmp');

% vypocty stredni hodnoty a smerodatne odchylky
% mezi step4 a step5
data1 = double(step4);
data2 = double(step5);
fprintf(results, 'mean_no_hist=%f\n', mean2(data1));
fprintf(results, 'std_no_hist=%f\n', std2(data1));
fprintf(results, 'mean_hist=%f\n', mean2(data2));
fprintf(results, 'std_hist=%f\n', std2(data2));
fclose(results);

% kvantizace obrazu
data1 = zeros(dimX, dimY);
%data2 = double(step5);
N = 2;
a = 0;
b = 255;

for (x = 1:dimX)
  for (y = 1:dimY)
    data1(x, y) = round(((2^N) - 1)*(data2(x, y) - a)/(b - a))*(b - a)/((2^N) - 1) + a;
  end;
end;

step6 = uint8(data1);
imwrite(step6, 'step6.bmp');
