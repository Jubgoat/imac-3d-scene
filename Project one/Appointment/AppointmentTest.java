import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.BeforeEach;
import java.util.Date;
import java.util.Calendar;
import static org.junit.jupiter.api.Assertions.*;

class AppointmentTest {
    private Date futureDate;
    private Date pastDate;

    @BeforeEach
    void setUp() {
        // Create a future date (tomorrow)
        Calendar futureCal = Calendar.getInstance();
        futureCal.add(Calendar.DAY_OF_YEAR, 1);
        futureDate = futureCal.getTime();

        // Create a past date (yesterday)
        Calendar pastCal = Calendar.getInstance();
        pastCal.add(Calendar.DAY_OF_YEAR, -1);
        pastDate = pastCal.getTime();
    }

    @Test
    void testAppointmentCreationValid() {
        Appointment appointment = new Appointment("APP1234567", futureDate, "Valid appointment description");
        
        assertEquals("APP1234567", appointment.getAppointmentId());
        assertEquals(futureDate, appointment.getAppointmentDate());
        assertEquals("Valid appointment description", appointment.getDescription());
    }

    @Test
    void testAppointmentIdNull() {
        assertThrows(IllegalArgumentException.class, () -> 
            new Appointment(null, futureDate, "Description"));
    }

    @Test
    void testAppointmentIdTooLong() {
        assertThrows(IllegalArgumentException.class, () -> 
            new Appointment("APP123456789", futureDate, "Description"));
    }

    @Test
    void testAppointmentIdExactly10Characters() {
        Appointment appointment = new Appointment("1234567890", futureDate, "Description");
        assertEquals("1234567890", appointment.getAppointmentId());
    }

    @Test
    void testAppointmentDateNull() {
        assertThrows(IllegalArgumentException.class, () -> 
            new Appointment("APP123", null, "Description"));
    }

    @Test
    void testAppointmentDateInPast() {
        assertThrows(IllegalArgumentException.class, () -> 
            new Appointment("APP123", pastDate, "Description"));
    }

    @Test
    void testAppointmentDateFuture() {
        Appointment appointment = new Appointment("APP123", futureDate, "Description");
        assertEquals(futureDate, appointment.getAppointmentDate());
    }

    @Test
    void testDescriptionNull() {
        assertThrows(IllegalArgumentException.class, () -> 
            new Appointment("APP123", futureDate, null));
    }

    @Test
    void testDescriptionTooLong() {
        String longDescription = "This description is definitely longer than fifty characters, which is not allowed.";
        assertThrows(IllegalArgumentException.class, () -> 
            new Appointment("APP123", futureDate, longDescription));
    }

    @Test
    void testDescriptionExactly50Characters() {
        String exactDescription = "12345678901234567890123456789012345678901234567890";
        Appointment appointment = new Appointment("APP123", futureDate, exactDescription);
        assertEquals(exactDescription, appointment.getDescription());
    }

    @Test
    void testUpdateAppointmentDateValid() {
        Appointment appointment = new Appointment("APP123", futureDate, "Initial description");
        
        // Create another future date
        Calendar newFutureCal = Calendar.getInstance();
        newFutureCal.add(Calendar.DAY_OF_YEAR, 2);
        Date newFutureDate = newFutureCal.getTime();
        
        appointment.setAppointmentDate(newFutureDate);
        assertEquals(newFutureDate, appointment.getAppointmentDate());
    }

    @Test
    void testUpdateAppointmentDateToPast() {
        Appointment appointment = new Appointment("APP123", futureDate, "Description");
        
        assertThrows(IllegalArgumentException.class, () -> 
            appointment.setAppointmentDate(pastDate));
    }

    @Test
    void testUpdateDescriptionValid() {
        Appointment appointment = new Appointment("APP123", futureDate, "Initial description");
        appointment.setDescription("Updated description");
        assertEquals("Updated description", appointment.getDescription());
    }

    @Test
    void testUpdateDescriptionToNull() {
        Appointment appointment = new Appointment("APP123", futureDate, "Initial description");
        assertThrows(IllegalArgumentException.class, () -> 
            appointment.setDescription(null));
    }

    @Test
    void testUpdateDescriptionTooLong() {
        Appointment appointment = new Appointment("APP123", futureDate, "Initial description");
        String longDescription = "This description is definitely longer than fifty characters, which is not allowed.";
        assertThrows(IllegalArgumentException.class, () -> 
            appointment.setDescription(longDescription));
    }
}