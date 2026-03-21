%% PROGETTO ESAME: CONTROLLO MOTORE DC
clear; close all; clc;

%% 1. IDENTIFICAZIONE SPERIMENTALE (Dati Reali)
% Dati raccolti col cronometro
PWM_meas = [60, 100, 150, 200, 255];
RPM_meas = [102, 198, 246, 270, 312];


% % Calcolo della retta di regressione (Modello Inverso per Arduino)
% % PWM = m * RPM + q
% p = polyfit(RPM_meas, PWM_meas, 1); 
% m = p(1); % Guadagno inverso (circa 0.83)
% q = p(2); % Offset (circa -24)

% Calcolo della retta: Invece di polyfit (che media su tutti i punti),
% si usano i parametri calcolati manualmente e messi su Arduino
% per garantire coerenza tra grafico, codice e tesina.
m = 0.83; 
q = -24; 

% Plot Risultati Identificazione
figure('Name', '1. Identificazione Sperimentale', 'Color', 'w');
RPM_vec = 0:350;
plot(RPM_meas, PWM_meas, 'rx', 'MarkerSize', 12, 'LineWidth', 2); hold on;
plot(RPM_vec, m*RPM_vec + q, 'b--', 'LineWidth', 1.5);
grid on;
title(['Identificazione: PWM = ' num2str(m, '%.2f') '*RPM ' num2str(q, '%.2f')]);
xlabel('Velocità [RPM]'); ylabel('PWM');
legend('Dati Misurati', 'Modello Lineare');

%% 2. MODELLISTICA E ANALISI IN FREQUENZA
% Parametri stimati del sistema fisico (Funzione di Trasferimento)
% G(s) = K_proc / (Tau*s + 1)
K_proc = 1.2;    % Guadagno statico (RPM/PWM) stimato dai dati
Tau = 0.15;      % Costante di tempo stimata (inerzia)

s = tf('s');
G_motor = K_proc / (Tau*s + 1);

% Visualizzazione Analisi
figure('Name', '2. Analisi in Frequenza e Stabilità', 'Color', 'w');

% Diagramma di Bode
subplot(2,1,1);
margin(G_motor); % Traccia Bode e calcola i margini automaticamente
grid on;
title('Diagramma di Bode (Sistema a Catena Aperta)');

% Luogo delle Radici
subplot(2,1,2);
rlocus(G_motor);
grid on;
title('Luogo delle Radici');
% Nota: Il sistema del primo ordine è sempre stabile se chiuso in retroazione

%% 3. PROGETTAZIONE PID E CONFRONTO SIMULAZIONE
% Calcolo teorico di un PID (se ci fosse l'encoder)
C_pid = pidtune(G_motor, 'PID');

% Parametri Simulazione
T_sim = 2; dt = 0.001; time = 0:dt:T_sim;
Ref = zeros(size(time)); Ref(time>0.1) = 150; % Gradino a 150 RPM

% A. Simulazione Feedforward (Progetto reale - Linea Rossa)
DeadZone = 52;
RPM_ff = zeros(size(time));
curr_rpm = 0;

for i = 2:length(time)
    % 1. Calcolo del PWM (Arduino)
    pwm_req = m * Ref(i) + q;
    
    % Saturazioni software e hardware
    if pwm_req < DeadZone && pwm_req > 0, pwm_req = 0; end 
    if pwm_req > 255, pwm_req = 255; end
    if pwm_req < 0, pwm_req = 0; end
    
    % 2. Gestione DISTURBO (Fisica)
    % Se sono dopo 1 secondo, applico un carico che riduce l'efficienza
    disturbo_load = 0;
    if time(i) > 1.0
        disturbo_load = 40; % Il motore perde 40 RPM a causa del dito
    end
    
    % 3. Dinamica Motore Corretta
    % Il motore cerca di raggiungere (Velocità_Teorica - Disturbo)
    rpm_target_fisico = (K_proc * pwm_req) - disturbo_load;
    
    % Integrazione (Eulero) con inerzia
    curr_rpm = curr_rpm + (dt/Tau) * (rpm_target_fisico - curr_rpm);
    
    RPM_ff(i) = curr_rpm;
end

% B. Simulazione PID (Ideale a catena chiusa - Linea Verde)
sys_cl = feedback(C_pid * G_motor, 1);
[RPM_pid, t_pid] = step(150 * sys_cl, time);

% Aggiunta disturbo manuale sul PID (Grafica)
disturbo_idx = find(time > 1.0, 1); 
% Il disturbo fa scendere la velocità, ma il PID la riporta su (esponenziale)
if ~isempty(disturbo_idx)
    disturbo_visual = (40 * exp(-(time(disturbo_idx:end)-1)/0.15))';
    RPM_pid(disturbo_idx:end) = RPM_pid(disturbo_idx:end) - disturbo_visual;
end

% Plot Confronto Finale
figure('Name', '3. Confronto Prestazioni', 'Color', 'w');
plot(time, Ref, 'k--', 'LineWidth', 1.5); hold on;
plot(time, RPM_ff, 'r', 'LineWidth', 2);
plot(time, RPM_pid, 'g', 'LineWidth', 2);
grid on;
title('Risposta al Gradino con Disturbo (t=1s)');
xlabel('Tempo [s]'); ylabel('RPM');
legend('Target (150 RPM)', 'Progetto (Feedforward)', 'Teorico con PID (Feedback)');
text(1.1, 100, '\leftarrow Disturbo non compensato', 'Color', 'red');
text(1.2, 160, '\leftarrow Disturbo compensato', 'Color', 'green');
ylim([0 200]);