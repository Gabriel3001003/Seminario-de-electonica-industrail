import streamlit as st
from pyModbusTCP.client import ModbusClient
import time

PLC_IP = "192.168.0.52"
PLC_PORT = 502

# Direcciones de los registros de los sensores ultrasónicos
ULTRA1_REG = 50
ULTRA2_REG = 60

# Inicializar session_state
if "ultra1" not in st.session_state:
    st.session_state.ultra1 = 0
if "ultra2" not in st.session_state:
    st.session_state.ultra2 = 0

st.title("📊 Dashboard Modbus TCP - Lectura de Ultrasonidos")

def client_action():
    return ModbusClient(host=PLC_IP, port=PLC_PORT, auto_open=True)

def leer_ultrasonicos():
    client = client_action()
    try:
        # Leer HR50 y HR60
        ultra1 = client.read_holding_registers(ULTRA1_REG, 1)
        ultra2 = client.read_holding_registers(ULTRA2_REG, 1)

        if ultra1:
            st.session_state.ultra1 = ultra1[0]
        if ultra2:
            st.session_state.ultra2 = ultra2[0]

    except Exception as e:
        st.error(f"Error al leer: {e}")
    finally:
        client.close()

# Botón para refrescar valores
if st.button("🔄 Leer sensores"):
    leer_ultrasonicos()

# Mostrar valores actuales
st.subheader("📖 Valores actuales")
col1, col2 = st.columns(2)
col1.metric("🔹 Ultrasonido 1 (HR50)", f"{st.session_state.ultra1} cm")
col2.metric("🔹 Ultrasonido 2 (HR60)", f"{st.session_state.ultra2} cm")

# Refresco automático cada 2s
time.sleep(2)
leer_ultrasonicos()
st.rerun()
