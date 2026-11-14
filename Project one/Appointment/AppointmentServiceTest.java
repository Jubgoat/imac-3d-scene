import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.BeforeEach;
import java.util.Date;
import java.util.Calendar;
import static org.junit.jupiter.api.Assertions.*;

class AppointmentServiceTest {
    private AppointmentService appointmentService;
    private Date futureDate;

    @BeforeEach
    void setUp() {
        appointmentService = new AppointmentService();
        
        // Create a future date
        Calendar calendar = Calendar.getInstance();
        calendar.add(Calendar.DAY_OF_YEAR, 1);
        futureDate = calendar.getTime();
    }

    @Test
    void testAddAppointment() {
        Appointment appointment = new Appointment("APP001", futureDate, "Test appointment");
        appointmentService.addAppointment(appointment);
        
        assertEquals(1, appointmentService.getAppointmentCount());
        assertNotNull(appointmentService.getAppointment("APP001"));
        assertEquals(appointment, appointmentService.getAppointment("APP001"));
    }

    @Test
    void testAddAppointmentWithNull() {
        assertThrows(IllegalArgumentException.class, () -> 
            appointmentService.addAppointment(null));
    }

    @Test
    void testAddAppointmentDuplicateId() {
        Appointment appointment1 = new Appointment("APP001", futureDate, "First appointment");
        Appointment appointment2 = new Appointment("APP001", futureDate, "Second appointment");
        
        appointmentService.addAppointment(appointment1);
        assertThrows(IllegalArgumentException.class, () -> 
            appointmentService.addAppointment(appointment2));
    }

    @Test
    void testAddMultipleAppointments() {
        Appointment appointment1 = new Appointment("APP001", futureDate, "First appointment");
        Appointment appointment2 = new Appointment("APP002", futureDate, "Second appointment");
        Appointment appointment3 = new Appointment("APP003", futureDate, "Third appointment");
        
        appointmentService.addAppointment(appointment1);
        appointmentService.addAppointment(appointment2);
        appointmentService.addAppointment(appointment3);
        
        assertEquals(3, appointmentService.getAppointmentCount());
        assertNotNull(appointmentService.getAppointment("APP001"));
        assertNotNull(appointmentService.getAppointment("APP002"));
        assertNotNull(appointmentService.getAppointment("APP003"));
    }

    @Test
    void testDeleteAppointment() {
        Appointment appointment = new Appointment("APP001", futureDate, "Test appointment");
        appointmentService.addAppointment(appointment);
        
        assertEquals(1, appointmentService.getAppointmentCount());
        
        appointmentService.deleteAppointment("APP001");
        
        assertEquals(0, appointmentService.getAppointmentCount());
        assertNull(appointmentService.getAppointment("APP001"));
    }

    @Test
    void testDeleteNonExistentAppointment() {
        assertThrows(IllegalArgumentException.class, () -> 
            appointmentService.deleteAppointment("NONEXISTENT"));
    }

    @Test
    void testDeleteAppointmentFromMultiple() {
        Appointment appointment1 = new Appointment("APP001", futureDate, "First appointment");
        Appointment appointment2 = new Appointment("APP002", futureDate, "Second appointment");
        Appointment appointment3 = new Appointment("APP003", futureDate, "Third appointment");
        
        appointmentService.addAppointment(appointment1);
        appointmentService.addAppointment(appointment2);
        appointmentService.addAppointment(appointment3);
        
        assertEquals(3, appointmentService.getAppointmentCount());
        
        appointmentService.deleteAppointment("APP002");
        
        assertEquals(2, appointmentService.getAppointmentCount());
        assertNotNull(appointmentService.getAppointment("APP001"));
        assertNull(appointmentService.getAppointment("APP002"));
        assertNotNull(appointmentService.getAppointment("APP003"));
    }

    @Test
    void testUpdateAppointment() {
        Appointment appointment = new Appointment("APP001", futureDate, "Initial description");
        appointmentService.addAppointment(appointment);
        
        // Create a new future date
        Calendar newCalendar = Calendar.getInstance();
        newCalendar.add(Calendar.DAY_OF_YEAR, 2);
        Date newFutureDate = newCalendar.getTime();
        
        appointmentService.updateAppointment("APP001", newFutureDate, "Updated description");
        
        Appointment updatedAppointment = appointmentService.getAppointment("APP001");
        assertEquals(newFutureDate, updatedAppointment.getAppointmentDate());
        assertEquals("Updated description", updatedAppointment.getDescription());
    }

    @Test
    void testUpdateNonExistentAppointment() {
        Calendar newCalendar = Calendar.getInstance();
        newCalendar.add(Calendar.DAY_OF_YEAR, 2);
        Date newFutureDate = newCalendar.getTime();
        
        assertThrows(IllegalArgumentException.class, () -> 
            appointmentService.updateAppointment("NONEXISTENT", newFutureDate, "Description"));
    }

    @Test
    void testUpdateAppointmentWithInvalidDate() {
        Appointment appointment = new Appointment("APP001", futureDate, "Initial description");
        appointmentService.addAppointment(appointment);
        
        // Create a past date
        Calendar pastCalendar = Calendar.getInstance();
        pastCalendar.add(Calendar.DAY_OF_YEAR, -1);
        Date pastDate = pastCalendar.getTime();
        
        assertThrows(IllegalArgumentException.class, () -> 
            appointmentService.updateAppointment("APP001", pastDate, "Updated description"));
    }

    @Test
    void testUpdateAppointmentWithInvalidDescription() {
        Appointment appointment = new Appointment("APP001", futureDate, "Initial description");
        appointmentService.addAppointment(appointment);
        
        String longDescription = "This description is definitely longer than fifty characters, which is not allowed.";
        
        assertThrows(IllegalArgumentException.class, () -> 
            appointmentService.updateAppointment("APP001", futureDate, longDescription));
    }

    @Test
    void testGetAppointmentCount() {
        assertEquals(0, appointmentService.getAppointmentCount());
        
        Appointment appointment1 = new Appointment("APP001", futureDate, "First appointment");
        appointmentService.addAppointment(appointment1);
        assertEquals(1, appointmentService.getAppointmentCount());
        
        Appointment appointment2 = new Appointment("APP002", futureDate, "Second appointment");
        appointmentService.addAppointment(appointment2);
        assertEquals(2, appointmentService.getAppointmentCount());
        
        appointmentService.deleteAppointment("APP001");
        assertEquals(1, appointmentService.getAppointmentCount());
    }

    @Test
    void testAppointmentPersistenceInService() {
        Appointment appointment = new Appointment("APP001", futureDate, "Test appointment");
        appointmentService.addAppointment(appointment);
        
        // Verify the appointment in service is the same object
        Appointment retrievedAppointment = appointmentService.getAppointment("APP001");
        assertSame(appointment, retrievedAppointment);
    }
}