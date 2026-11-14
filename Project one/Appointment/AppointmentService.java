import java.util.HashMap;
import java.util.Map;

public class AppointmentService {
    private final Map<String, Appointment> appointments;

    public AppointmentService() {
        this.appointments = new HashMap<>();
    }

    /**
     * Adds an appointment to the service
     * @param appointment The appointment to add
     * @throws IllegalArgumentException if appointment is null or ID already exists
     */
    public void addAppointment(Appointment appointment) {
        if (appointment == null) {
            throw new IllegalArgumentException("Appointment cannot be null");
        }

        String appointmentId = appointment.getAppointmentId();
        if (appointments.containsKey(appointmentId)) {
            throw new IllegalArgumentException("Appointment ID already exists: " + appointmentId);
        }

        appointments.put(appointmentId, appointment);
    }

    /**
     * Deletes an appointment by ID
     * @param appointmentId The ID of the appointment to delete
     * @throws IllegalArgumentException if appointment ID doesn't exist
     */
    public void deleteAppointment(String appointmentId) {
        if (!appointments.containsKey(appointmentId)) {
            throw new IllegalArgumentException("Appointment ID not found: " + appointmentId);
        }

        appointments.remove(appointmentId);
    }

    /**
     * Gets an appointment by ID
     * @param appointmentId The ID of the appointment to retrieve
     * @return The appointment, or null if not found
     */
    public Appointment getAppointment(String appointmentId) {
        return appointments.get(appointmentId);
    }

    /**
     * Updates an existing appointment
     * @param appointmentId The ID of the appointment to update
     * @param newAppointmentDate The new appointment date
     * @param newDescription The new description
     * @throws IllegalArgumentException if appointment ID doesn't exist
     */
    public void updateAppointment(String appointmentId, java.util.Date newAppointmentDate, String newDescription) {
        Appointment appointment = getAppointment(appointmentId);
        if (appointment == null) {
            throw new IllegalArgumentException("Appointment ID not found: " + appointmentId);
        }

        appointment.setAppointmentDate(newAppointmentDate);
        appointment.setDescription(newDescription);
    }

    /**
     * Gets the number of appointments in the service
     * @return The count of appointments
     */
    public int getAppointmentCount() {
        return appointments.size();
    }
}