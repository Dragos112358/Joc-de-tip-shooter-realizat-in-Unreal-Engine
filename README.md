# CPP_Shooter2 - Unreal Engine 5 Tactical Shooter

Acesta este un proiect de tip Shooter dezvoltat în **Unreal Engine 5**, care pune accent pe mecanici realiste de luptă, explorare cu vehicule și un sistem robust de progresie prin realizări (achievements). Proiectul este conceput cu o hartă diversificată, oferind un mix între luptă urbană și tactici de tip sniper în zone deschise.

---

## Funcționalități Principale (Features)

* ** Arsenal Diversificat:** Sistem de luptă integrat pentru **13 arme distincte** (AK-47, SCAR, etc.), cu mecanici de recoil, damage drop-off și gestionare a muniției.
* ** Sistem de Vehicule:** Vehicul complet funcțional (Chaos Physics) pentru navigarea rapidă pe harta vastă.
* ** Sistem de Achievements:** Logica implementată pentru monitorizarea performanței jucătorului și deblocarea de realizări specifice în timpul sesiunii de joc.
* ** Management de Resurse:** Sistem de inventar pentru muniție, forțând jucătorul să exploreze mediul pentru supraviețuire.
* ** Level Design Avansat:** * Zonă urbană densă (peste 30 de clădiri/blocuri explorabile).
    * Relief variat: lac masiv și lanț muntos pentru gameplay vertical.

---

## Detalii Tehnice

* **Motor Grafic:** Unreal Engine 5.x
* **Limbaj principal:** C++ și Blueprints (Arhitectură hibridă)
* **Mecanici cheie:** * Line Tracing pentru sistemul de shooting.
    * Component-based design pentru arme și vehicule.
    * Sistem de salvare (SaveGame) pentru persistența achievement-urilor.

---

## Cum se instalează și rulează

Pentru a rula acest proiect local, urmează pașii de mai jos:

1.  **Cerințe:** Asigură-te că ai instalat **Unreal Engine 5** și **Visual Studio 2022** (cu pachetele C++ pentru Game Development).
2.  **Clonare:**
    ```bash
    git clone [https://github.com/Dragos112358/Joc-de-tip-shooter-realizat-in-Unreal-Engine.git](https://github.com/Dragos112358/Joc-de-tip-shooter-realizat-in-Unreal-Engine.git)
    ```
3.  **Generare fișiere:** Dă click dreapta pe fișierul `CPP_Shooter2.uproject` și selectează **"Generate Visual Studio project files"**.
4.  **Compilare:** Deschide fișierul `.sln` în Visual Studio, setează configurația pe **Development Editor** și apasă **Build Solution**.
5.  **Lansare:** Deschide `.uproject` și apasă **Play** în editorul Unreal.


Acest proiect este creat în scop educațional și de portofoliu.
