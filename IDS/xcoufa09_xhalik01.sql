-- Project IDS databases script
-- Authors:
--    Roman Halík xhalik01@stud.fit.vutbr.cz
--    Tomáš Coufal xcoufa09@stud.fit.vutbr.cz
-- drop all tables
------------------------------------------------------------------------------
drop table Zdravotni_sestra cascade constraints;
drop table Lekar cascade constraints;
drop table Personal cascade constraints;
drop table Lekar_pracuje_na_Oddeleni cascade constraints;
drop table Vysetreni cascade constraints;
drop table Vysetreni_aplikuje_Lecivo cascade constraints;
drop table Oddeleni cascade constraints;
drop table Odd_poskytuje_hosp cascade constraints;
drop table Pacient cascade constraints;
drop table Hospitalizace cascade constraints;
drop table Lecivo cascade constraints;
drop table Hosp_uziva_Lecivo cascade constraints;

drop sequence personal_seq;
drop sequence pacient_seq;
drop sequence hospitalizace_seq;
drop sequence vysetreni_seq;

-- (re)create tables
------------------------------------------------------------------------------
-- Table Personal
------------------------------------------------------------------------------
create table Personal (
  ID_personal integer not null,
  login varchar(10) not null,
  heslo varchar(256) not null,
  rodne_cislo integer not null check (mod(rodne_cislo,11) = 0),
  jmeno nvarchar2(256) not null,
  prijmeni nvarchar2(256) not null,
  adresa nvarchar2(512) not null,
  telefon varchar(13),
  email varchar(256),
  uvazek float not null check (uvazek > 0 and uvazek <= 1)
);
alter table Personal add (primary key (ID_personal));

-- Table Zdravotni Sestra
------------------------------------------------------------------------------
create table Zdravotni_sestra (
  ID_personal integer not null,
  kvalifikace nvarchar2(128),
  prirazena_k_oddeleni nvarchar2(256) not null
);

-- Table Lekar
------------------------------------------------------------------------------
create table Lekar (
  ID_personal integer not null,
  specializace nvarchar2(128) not null
);

-- Table Oddeleni
------------------------------------------------------------------------------
create table Oddeleni (
  nazev_oddeleni nvarchar2(256) not null,
  telefon_na_sesternu varchar(13) not null,
  kapacita integer not null,
  vrchni_sestra integer -- sestra
);
alter table Oddeleni add (primary key (nazev_oddeleni));

-- Table Vysetreni
------------------------------------------------------------------------------
create table Vysetreni (
  ID_vysetreni integer not null,
  druh nvarchar2(512) not null,
  cas date not null,
  datum date not null,
  vysledek nvarchar2(1024) not null,
  provedl integer not null,
  oddeleni nvarchar2(256) not null,
  hospitalizace integer not null
);
alter table Vysetreni add (primary key (ID_vysetreni));

-- Table Lecivo
------------------------------------------------------------------------------
create table Lecivo (
  katalogove_oznaceni varchar(128) not null,
  nazev nvarchar2(256) not null,
  ucinna_latka varchar(256) not null,
  kotraindikace nvarchar2(512),
  doporucene_davkovani varchar(128),
  mnozstvi_ucinne_latky varchar(128)
);
alter table Lecivo add (primary key (katalogove_oznaceni));

-- Table Pacient
------------------------------------------------------------------------------
create table Pacient (
  ID_pacienta integer not null,
  rodne_cislo integer not null check (mod(rodne_cislo,11) = 0),
  jmeno nvarchar2(256) not null,
  prijmeni nvarchar2(256) not null,
  pojistovna integer not null,
  adresa nvarchar2(512),
  telefon_na_pribuzne varchar(13) not null,
  telefon varchar(13),
  email varchar(256)
);
alter table Pacient add (primary key (ID_pacienta));

-- Table Hospitalizace
------------------------------------------------------------------------------
create table Hospitalizace (
  ID_hospitalizace integer not null,
  datum_zahajeni date not null,
  datum_ukonceni date not null,
  pacient integer not null,
  oddeleni nvarchar2(256) not null,
  dozorujici_lekar integer not null
);
alter table Hospitalizace add (primary key (ID_hospitalizace));
alter table Hospitalizace add constraint chk_datum_hospitalizace check (datum_zahajeni <= datum_ukonceni);

-- Relation table Lekar Pracuje na oddeleni
------------------------------------------------------------------------------
create table Lekar_pracuje_na_Oddeleni (
  lekar integer not null,
  oddeleni nvarchar2(256) not null,
  telefon varchar(13) not null,
  uvazek float not null -- soucet uroven uvazku Personal
);

-- Relation table Vysetreni aplikuje lecivo
------------------------------------------------------------------------------
create table Vysetreni_aplikuje_Lecivo (
  vysetreni integer not null,
  lecivo varchar(256) not null,
  mnozstvi integer not null,
  cas date not null,
  datum date null
);

-- Relation table Oddeleni poskytuje hospitalizaci
------------------------------------------------------------------------------
create table Odd_poskytuje_hosp (
  oddeleni nvarchar2(256) not null,
  hospitalizace integer not null,
  datum_zahajeni date not null,
  datum_ukonceni date not null
);
alter table Odd_poskytuje_hosp add constraint chk_datum_poskyt check (datum_zahajeni <= datum_ukonceni);

-- Relation table Hospitalizace uziva lecivo
------------------------------------------------------------------------------
create table Hosp_uziva_Lecivo (
  hospitalizace integer not null,
  lecivo varchar(256) not null,
  frekvence varchar(128) not null,
  mnozstvi varchar(128) not null
);

-- Foreign keys
------------------------------------------------------------------------------
alter table Lekar add (foreign key (ID_personal) references Personal (ID_personal));
alter table Zdravotni_sestra add (foreign key (ID_personal) references Personal (ID_personal));
alter table Zdravotni_sestra add (foreign key (prirazena_k_oddeleni) references Oddeleni (nazev_oddeleni));
alter table Oddeleni add (foreign key (vrchni_sestra) references Personal (ID_personal));
alter table Lekar_pracuje_na_Oddeleni add (foreign key (lekar) references Personal (ID_personal));
alter table Lekar_pracuje_na_Oddeleni add (foreign key (oddeleni) references Oddeleni (nazev_oddeleni));
alter table Vysetreni add (foreign key (provedl) references Personal (ID_personal));
alter table Vysetreni add (foreign key (oddeleni) references Oddeleni (nazev_oddeleni));
alter table Vysetreni add (foreign key (hospitalizace) references Hospitalizace (ID_hospitalizace));
alter table Vysetreni_aplikuje_Lecivo add (foreign key (vysetreni) references Vysetreni (ID_vysetreni));
alter table Vysetreni_aplikuje_Lecivo add (foreign key (lecivo) references Lecivo (katalogove_oznaceni));
alter table Hospitalizace add (foreign key (pacient) references Pacient (ID_pacienta));
alter table Hospitalizace add (foreign key (oddeleni) references Oddeleni (nazev_oddeleni));
alter table Hospitalizace add (foreign key (dozorujici_lekar) references Personal (ID_personal));
alter table Odd_poskytuje_hosp add (foreign key (oddeleni) references Oddeleni (nazev_oddeleni));
alter table Odd_poskytuje_hosp add (foreign key (hospitalizace) references Hospitalizace (ID_hospitalizace));
alter table Hosp_uziva_Lecivo add (foreign key (hospitalizace) references Hospitalizace (ID_hospitalizace));
alter table Hosp_uziva_Lecivo add (foreign key (lecivo) references Lecivo (katalogove_oznaceni));

-- Create seqvences
------------------------------------------------------------------------------
create sequence personal_seq increment by 1 start with 1 nomaxvalue minvalue 1 nocycle;
create sequence pacient_seq increment by 1 start with 1 nomaxvalue minvalue 1 nocycle;
create sequence hospitalizace_seq increment by 1 start with 1 nomaxvalue minvalue 1 nocycle;
create sequence vysetreni_seq increment by 1 start with 1 nomaxvalue minvalue 1 nocycle;

-- Insert data
------------------------------------------------------------------------------
insert into Personal (ID_personal, login, heslo, rodne_cislo, jmeno, prijmeni, adresa, telefon, email, uvazek)
       values (personal_seq.nextval,'novak01','heslonovak',8709051835,'Pavel','Novák','Lamerova 19, Brno','+420587447129','novak@seznam.cz',1);
insert into Lekar (ID_personal, specializace)
      values (1,'Chirurg');
insert into Personal (ID_personal, login, heslo, rodne_cislo, jmeno, prijmeni, adresa, telefon, email, uvazek)
       values (personal_seq.nextval,'vesel22','heslovesely',8709051835,'Pete','Veselý','Břídilova 60, Brno, Židenice','+420112365418','vesely@seznam.cz',0.5);
insert into Lekar (ID_personal, specializace)
       values (2, 'Kardiolog');
insert into Personal (ID_personal, login, heslo, rodne_cislo, jmeno, prijmeni, adresa, telefon, email, uvazek)
       values (personal_seq.nextval,'zeman41','heslozeman',8701046882,'Ivan','Zeman','Pupkáčova 20, Brno, Tuřany','+420884154632','zeman@centrum.cz',1);
insert into Lekar (ID_personal, specializace)
      values (3, 'Internista');

insert into Oddeleni (nazev_oddeleni, telefon_na_sesternu, kapacita)
       values ('Kardiologie','324571456',30);
insert into Oddeleni (nazev_oddeleni, telefon_na_sesternu, kapacita)
       values ('Chirurgie','625447897',62);

insert into Personal (ID_personal, login, heslo, rodne_cislo, jmeno, prijmeni, adresa, telefon, email, uvazek)
       values (personal_seq.nextval,'novot38','heslonovotna',8751152861,'Martina','Novotná','Banskobystrická 1, Brno, Mokrá Hora','+420855124758','martina.novotna@centrum.cz',1);
insert into Zdravotni_sestra (ID_personal, kvalifikace, prirazena_k_oddeleni)
       values (4,'Ošetrovatelství', 'Kardiologie');
insert into Personal (ID_personal, login, heslo, rodne_cislo, jmeno, prijmeni, adresa, telefon, email, uvazek)
       values (personal_seq.nextval,'novot39','heslonovotna',8754270679,'Marta','Novotná','Banskobystrická 2, Brno, Mokrá Hora','+420754921723','marta.novotna@centrum.cz',1);
insert into Zdravotni_sestra (ID_personal, prirazena_k_oddeleni)
       values (5, 'Chirurgie');
insert into Personal (ID_personal, login, heslo, rodne_cislo, jmeno, prijmeni, adresa, telefon, email, uvazek)
       values (personal_seq.nextval,'ceska11','hesloceska',8761235813,'Františka','Česká','Slovenská 1435, Nové Město','+420654657830','franta.ceska@email.cz',0.7);
insert into Zdravotni_sestra (ID_personal, prirazena_k_oddeleni)
       values (6,'Chirurgie');
insert into Personal (ID_personal, login, heslo, rodne_cislo, jmeno, prijmeni, adresa, telefon, email, uvazek)
       values (personal_seq.nextval,'petra13','heslopetra',8758184644,'Petra','Petrovičová','Ulice 12, Staré město','+420609873871','mujnovymail@seznam.cz',1);
insert into Zdravotni_sestra (ID_personal, kvalifikace, prirazena_k_oddeleni)
       values (7,'Děti a porodnicví','Chirurgie');
insert into Personal (ID_personal, login, heslo, rodne_cislo, jmeno, prijmeni, adresa, telefon, email, uvazek)
       values (personal_seq.nextval,'rudolf1','tajneheslo',8704205620,'Rudolf','Zelený','Za komínem 3, Brno','+420605647846','jajsemruda@seznam.cz',1);
insert into Zdravotni_sestra (ID_personal, kvalifikace, prirazena_k_oddeleni)
       values (8,'Balzamování','Kardiologie');

--update --prirad vrchni sestru
update Oddeleni set vrchni_sestra = 4 where nazev_oddeleni = 'Kardiologie';
update Oddeleni set vrchni_sestra = 5 where nazev_oddeleni = 'Chirurgie';

insert into pacient (ID_pacienta,rodne_cislo,jmeno,prijmeni,pojistovna,adresa,telefon_na_pribuzne,telefon,email)
       values(pacient_seq.nextval,9308058815,'Jan','Chudý',111,'Netopilova 1, Brno, Medlánky','+420454774865','+420777568885','chudyjan@seznam.cz');
insert into pacient (ID_pacienta,rodne_cislo,jmeno,prijmeni,pojistovna,adresa,telefon_na_pribuzne,telefon,email)
       values(pacient_seq.nextval,6904185662,'Marek','Hradil',111,'Rooseveltova 47, Brno','+420454774844','+420123456865','m.hradil@gmail.com');
insert into pacient (ID_pacienta,rodne_cislo,jmeno,prijmeni,pojistovna,adresa,telefon_na_pribuzne,telefon,email)
       values(pacient_seq.nextval,8903155349,'Marek','Dobeš',201,'Jánská 47, Brno','+420456578844','+420122356865','dobesdobes@gmail.com');
insert into pacient (ID_pacienta,rodne_cislo,jmeno,prijmeni,pojistovna,adresa,telefon_na_pribuzne,telefon,email)
       values(pacient_seq.nextval,7262023516,'Lucie','Kolaříková',205,'Hečíkova 11, Brno','+420453635644','+420857545865','kolarikova.lucinka@seznam.cz');
insert into pacient (ID_pacienta,rodne_cislo,jmeno,prijmeni,pojistovna,adresa,telefon_na_pribuzne,telefon,email)
       values(pacient_seq.nextval,8957281729,'Denisa','Moravcová',201,'Jaselská 70, Brno','+420985774844','+420369587825','deniskamoravcova@gmail.com');
insert into pacient (ID_pacienta,rodne_cislo,jmeno,prijmeni,pojistovna,adresa,telefon_na_pribuzne,telefon,email)
       values(pacient_seq.nextval,6904185662,'Michal','Hradil',111,'Pražská 17, Brno','+420492874844','+420777456865','michal.hradil@gmail.com');

insert into hospitalizace (ID_hospitalizace,datum_zahajeni,datum_ukonceni,pacient,oddeleni,dozorujici_lekar)
       values(hospitalizace_seq.nextval,TO_DATE('10.10.2008', 'dd.mm.yyyy'),TO_DATE('10.11.2012', 'dd.mm.yyyy'),1,'Kardiologie',1);
insert into hospitalizace (ID_hospitalizace,datum_zahajeni,datum_ukonceni,pacient,oddeleni,dozorujici_lekar)
       values(hospitalizace_seq.nextval,TO_DATE('30.10.2008', 'dd.mm.yyyy'),TO_DATE('10.12.2010', 'dd.mm.yyyy'),2,'Chirurgie',3);
insert into hospitalizace (ID_hospitalizace,datum_zahajeni,datum_ukonceni,pacient,oddeleni,dozorujici_lekar)
       values(hospitalizace_seq.nextval,TO_DATE('12.10.2008', 'dd.mm.yyyy'),TO_DATE('11.10.2015', 'dd.mm.yyyy'),3,'Kardiologie',3);
insert into hospitalizace (ID_hospitalizace,datum_zahajeni,datum_ukonceni,pacient,oddeleni,dozorujici_lekar)
       values(hospitalizace_seq.nextval,TO_DATE('12.10.2008', 'dd.mm.yyyy'),TO_DATE('11.11.2015', 'dd.mm.yyyy'),4,'Kardiologie',1);
insert into hospitalizace (ID_hospitalizace,datum_zahajeni,datum_ukonceni,pacient,oddeleni,dozorujici_lekar)
       values(hospitalizace_seq.nextval,TO_DATE('12.10.2008', 'dd.mm.yyyy'),TO_DATE('11.11.2015', 'dd.mm.yyyy'),5,'Kardiologie',2);
insert into hospitalizace (ID_hospitalizace,datum_zahajeni,datum_ukonceni,pacient,oddeleni,dozorujici_lekar)
       values(hospitalizace_seq.nextval,TO_DATE('12.10.2013', 'dd.mm.yyyy'),TO_DATE('11.11.2015', 'dd.mm.yyyy'),6,'Chirurgie',1);

insert into vysetreni (ID_vysetreni,druh,cas,datum,vysledek,provedl,oddeleni,hospitalizace)
       values (vysetreni_seq.nextval,'druh_vysetreni',TO_DATE('13:37', 'HH24:MI'),TO_DATE('10.10.2008', 'dd.mm.yyyy'),'OK',2,'Kardiologie',1);
insert into vysetreni (ID_vysetreni,druh,cas,datum,vysledek,provedl,oddeleni,hospitalizace)
       values(vysetreni_seq.nextval,'druh_vysetreni',TO_DATE('11:17', 'HH24:MI'),TO_DATE('10.11.2008', 'dd.mm.yyyy'),'OK',1,'Chirurgie',1);
insert into vysetreni (ID_vysetreni,druh,cas,datum,vysledek,provedl,oddeleni,hospitalizace)
       values(vysetreni_seq.nextval,'druh_vysetreni',TO_DATE('14:51', 'HH24:MI'),TO_DATE('10.12.2008', 'dd.mm.yyyy'),'OK',2,'Kardiologie',1);

insert into lecivo (katalogove_oznaceni,nazev,ucinna_latka,kotraindikace,doporucene_davkovani,mnozstvi_ucinne_latky)
       values('0059393','0,9% SODIUM CHLORIDE INTRAVENOUS INFUSION BP BAXTER','CHLORID SODNÝ (NATRII CHLORIDUM)','diabetická ketoacidóza','2-0-2','0.9%');
insert into lecivo (katalogove_oznaceni,nazev,ucinna_latka,kotraindikace,doporucene_davkovani,mnozstvi_ucinne_latky)
       values('0115528','4-DMAP','DIMETHYLAMINOFENOL-HYDROCHLORID (DIMETHYLAMINOPHENOLI HYDROCHLORIDUM)','makroskopická hematurie','2-0-2','50MG/ML');
insert into lecivo (katalogove_oznaceni,nazev,ucinna_latka,kotraindikace,doporucene_davkovani,mnozstvi_ucinne_latky)
       values('0026314','ACTOS 45 MG','PIOGLITAZON-HYDROCHLORID (PIOGLITAZONI HYDROCHLORIDUM)','rakovina močového měchýře','1-0-2','45MG');
insert into lecivo (katalogove_oznaceni,nazev,ucinna_latka,kotraindikace,doporucene_davkovani,mnozstvi_ucinne_latky)
       values('0127548','(18F)FDG-FR','FLUDEOXYGLUKOSA-(18F) (FLUDEOXYGLUCOSUM (18F))','porucha funkce jater','1-1-2','0.3-3.1GB/ML');

insert into Lekar_pracuje_na_Oddeleni (lekar,oddeleni,telefon,uvazek)
       values(1,'Chirurgie','625447897',1);
insert into Lekar_pracuje_na_Oddeleni (lekar,oddeleni,telefon,uvazek)
       values(1,'Kardiologie','324571456',1);
insert into Lekar_pracuje_na_Oddeleni (lekar,oddeleni,telefon,uvazek)
       values(2,'Kardiologie','324571456',0.5);
insert into Lekar_pracuje_na_Oddeleni (lekar,oddeleni,telefon,uvazek)
       values(3,'Kardiologie','324571456',1);

insert into Vysetreni_aplikuje_Lecivo (vysetreni,lecivo,mnozstvi,cas,datum)
       values(1,'0059393','2',TO_DATE('10:10', 'HH24:MI'),TO_DATE('10.10.2008', 'dd.mm.yyyy'));
insert into Vysetreni_aplikuje_Lecivo (vysetreni,lecivo,mnozstvi,cas,datum)
       values(2,'0115528','4',TO_DATE('13:33', 'HH24:MI'),TO_DATE('10.11.2008', 'dd.mm.yyyy'));
insert into Vysetreni_aplikuje_Lecivo (vysetreni,lecivo,mnozstvi,cas,datum)
       values(1,'0026314','1',TO_DATE('11:11', 'HH24:MI'),TO_DATE('10.12.2008', 'dd.mm.yyyy'));

insert into Odd_poskytuje_hosp (oddeleni,hospitalizace,datum_zahajeni,datum_ukonceni)
       values('Kardiologie',1,TO_DATE('10.10.2008', 'dd.mm.yyyy'),TO_DATE('10.11.2012', 'dd.mm.yyyy'));
insert into Odd_poskytuje_hosp (oddeleni,hospitalizace,datum_zahajeni,datum_ukonceni)
       values('Chirurgie',2,TO_DATE('30.10.2008', 'dd.mm.yyyy'),TO_DATE('10.12.2010', 'dd.mm.yyyy'));
insert into Odd_poskytuje_hosp (oddeleni,hospitalizace,datum_zahajeni,datum_ukonceni)
       values('Kardiologie',3,TO_DATE('12.10.2008', 'dd.mm.yyyy'),TO_DATE('11.10.2009', 'dd.mm.yyyy'));

insert into Hosp_uziva_Lecivo (hospitalizace,lecivo,frekvence,mnozstvi)
       values(1,'0059393','3x denně','2 tablety');
insert into Hosp_uziva_Lecivo (hospitalizace,lecivo,frekvence,mnozstvi)
       values(2,'0059393','1 za rok','1');
insert into Hosp_uziva_Lecivo (hospitalizace,lecivo,frekvence,mnozstvi)
       values(3,'0026314','2x denně','1 balení');
commit;

--------------------------------------------------------------------------------------------
---------------------------------- 3. cast - dotazy SELECT ---------------------------------
--------------------------------------------------------------------------------------------
-- dva dotazy využívající spojení dvou tabulek,
-- jeden využívající spojení tří tabulek, 
-- dva dotazy s klauzulí GROUP BY a agregační funkcí,
-- jeden dotaz obsahující predikát EXISTS,
-- jeden dotaz s predikátem IN s vnořeným selectem (nikoliv IN s množinou konstatních dat). 

-- U každého z dotazů musí být (v komentáři SQL kódu) popsáno srozumitelně, 
-- jaká data hledá daný dotaz (jaká je jeho funkce v aplikaci).

-- 1) spojeni dvou tabulek
-- vypise jmeno a prijmeni a ID vysetreni vsech zamestnancu, kteri provedli nejaka vysetreni
  select jmeno, prijmeni,ID_vysetreni  
  from Personal, vysetreni 
  where (Personal.ID_personal = vysetreni.provedl);

-- vypise jmeno, prijmeni a pojistovnu pacientu, kteri byli hospitalizovani na oddeleni Kardiologie
  select jmeno, prijmeni, pojistovna 
  from Pacient, Hospitalizace
  where (pacient.ID_pacienta = Hospitalizace.pacient AND Hospitalizace.oddeleni = 'Kardiologie');

-- 3) spojeni 3 tabulek
-- vypise vsechna oddeleni, na kterych pracuje lekar s ID 1
  select ID_personal, nazev_oddeleni 
  from (Lekar inner join Lekar_pracuje_na_Oddeleni ON ID_personal = lekar inner join Oddeleni on Oddeleni = nazev_oddeleni) 
  where ID_personal = 1;

-- 4) Klaule group by s agregacni funkci
-- vypise, kolik zamestnancu ma dany typ uvazku
  select COUNT(ID_personal), uvazek from Personal GROUP BY uvazek;

-- pocet aktualne hospitalizovanyc pacientu
  select SUM(COUNT(ID_pacienta)) 
  from pacient
  inner join hospitalizace on (hospitalizace.pacient = pacient.ID_pacienta) 
  where(SYSDATE BETWEEN hospitalizace.datum_zahajeni and hospitalizace.datum_ukonceni) 
  group by ID_pacienta;

-- 5) Dotaz obsahujici predikat EXISTS
-- vyber neco, pokud nekde jinde plati, ze to existuje
-- vymyslet rozumny priklad.. vse co me napada je strasne vykonstruovane a nema to smysl takto psat

-- 6) dotaz s predikatem IN s vnorenym selectem
-- vypise jmeno, prijmeni a pojistovnu pacietu, kteri jsou prave hospitalizovani a jejich pojistovna je 111
select jmeno, prijmeni, pojistovna from pacient inner join hospitalizace on (ID_pacienta = pacient) 
where(SYSDATE BETWEEN hospitalizace.datum_zahajeni and hospitalizace.datum_ukonceni) and pojistovna in (select pojistovna from pacient where pojistovna = 111);
