/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2009. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_XSI_MESSAGE_QUEUE_HPP
#define BOOST_INTERPROCESS_XSI_MESSAGE_QUEUE_HPP

#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/interprocess/xsi_shared_memory.hpp>
#include <boost/interprocess/detail/xsi_shared_memory_file_wrapper.hpp>

//!\file
//!Describes an inter-process message queue using XSI shared memory. 
//!This class allows sending messages between processes and allows 
//!blocking, non-blocking and timed sending and receiving.

namespace boost{  namespace interprocess{

//!A class that allows sending messages
//!between processes, using XSI shared memory.
class xsi_message_queue
{
   /// @cond
   //Blocking modes
   enum block_t   {  blocking,   timed,   non_blocking   };

   xsi_message_queue();
   /// @endcond

   public:

   //!Creates a process shared message queue with a given XSI key. For this message queue,
   //!the maximum number of messages will be "max_num_msg" and the maximum message size
   //!will be "max_msg_size". Throws on error and if the queue was previously created.
   xsi_message_queue(create_only_t create_only,
		 xsi_key key,
                 std::size_t max_num_msg, 
                 std::size_t max_msg_size,
                 const permissions &perm = permissions());

   //!Opens or creates a process shared message queue with a given XSI key. 
   //!If the queue is created, the maximum number of messages will be "max_num_msg" 
   //!and the maximum message size will be "max_msg_size". If queue was previously 
   //!created the queue will be opened and "max_num_msg" and "max_msg_size" parameters
   //!are ignored. Throws on error.
   xsi_message_queue(open_or_create_t open_or_create,
		 xsi_key key, 
                 std::size_t max_num_msg, 
                 std::size_t max_msg_size,
                 const permissions &perm = permissions());

   //!Opens a previously created process shared message queue with a given XSI key. 
   //!If the was not previously created or there are no free resources, 
   //!throws an error.
   xsi_message_queue(open_only_t open_only,
                 xsi_key key); 

   //!Destroys *this and indicates that the calling process is finished using
   //!the resource. All opened message queues are still
   //!valid after destruction. The destructor function will deallocate
   //!any system resources allocated by the system for use by this process for
   //!this resource. The resource can still be opened again calling
   //!the open constructor overload. To erase the message queue from the system
   //!use remove().
   ~xsi_message_queue(); 

   //!Sends a message stored in buffer "buffer" with size "buffer_size" in the 
   //!message queue with priority "priority". If the message queue is full
   //!the sender is blocked. Throws interprocess_error on error.*/
   void send (const void *buffer,     std::size_t buffer_size, 
              unsigned int priority);

   //!Sends a message stored in buffer "buffer" with size "buffer_size" through the 
   //!message queue with priority "priority". If the message queue is full
   //!the sender is not blocked and returns false, otherwise returns true.
   //!Throws interprocess_error on error.
   bool try_send    (const void *buffer,     std::size_t buffer_size, 
                         unsigned int priority);

   //!Sends a message stored in buffer "buffer" with size "buffer_size" in the 
   //!message queue with priority "priority". If the message queue is full
   //!the sender retries until time "abs_time" is reached. Returns true if
   //!the message has been successfully sent. Returns false if timeout is reached.
   //!Throws interprocess_error on error.
   bool timed_send    (const void *buffer,     std::size_t buffer_size, 
                           unsigned int priority,  const boost::posix_time::ptime& abs_time);

   //!Receives a message from the message queue. The message is stored in buffer 
   //!"buffer", which has size "buffer_size". The received message has size 
   //!"recvd_size" and priority "priority". If the message queue is empty
   //!the receiver is blocked. Throws interprocess_error on error.
   void receive (void *buffer,           std::size_t buffer_size, 
                 std::size_t &recvd_size,unsigned int &priority);

   //!Receives a message from the message queue. The message is stored in buffer 
   //!"buffer", which has size "buffer_size". The received message has size 
   //!"recvd_size" and priority "priority". If the message queue is empty
   //!the receiver is not blocked and returns false, otherwise returns true.
   //!Throws interprocess_error on error.
   bool try_receive (void *buffer,           std::size_t buffer_size, 
                     std::size_t &recvd_size,unsigned int &priority);

   //!Receives a message from the message queue. The message is stored in buffer 
   //!"buffer", which has size "buffer_size". The received message has size 
   //!"recvd_size" and priority "priority". If the message queue is empty
   //!the receiver retries until time "abs_time" is reached. Returns true if
   //!the message has been successfully sent. Returns false if timeout is reached.
   //!Throws interprocess_error on error.
   bool timed_receive (void *buffer,           std::size_t buffer_size, 
                       std::size_t &recvd_size,unsigned int &priority,
                       const boost::posix_time::ptime &abs_time);

   //!Returns the maximum number of messages allowed by the queue. The message
   //!queue must be opened or created previously. Otherwise, returns 0. 
   //!Never throws
   std::size_t get_max_msg() const;

   //!Returns the maximum size of message allowed by the queue. The message
   //!queue must be opened or created previously. Otherwise, returns 0. 
   //!Never throws
   std::size_t get_max_msg_size() const;

   //!Returns the number of messages currently stored. 
   //!Never throws
   std::size_t get_num_msg();

   //!Removes the message queue from the system.
   //!Returns false on error. Never throws
   static bool remove(xsi_key key);

   /// @cond   
   private:
   typedef boost::posix_time::ptime ptime;
   bool do_receive(block_t block,
                   void *buffer,            std::size_t buffer_size, 
                   std::size_t &recvd_size, unsigned int &priority,
                   const ptime &abs_time);

   bool do_send(block_t block,
                const void *buffer,      std::size_t buffer_size, 
                unsigned int priority,   const ptime &abs_time);

   //!Returns the needed memory size for the shared message queue.
   //!Never throws
   static std::size_t get_mem_size(std::size_t max_msg_size, std::size_t max_num_msg);

   detail::managed_open_or_create_impl<xsi_shared_memory_file_wrapper,false,true> m_shmem;
	/// @endcond
};

inline xsi_message_queue::~xsi_message_queue()
{}

inline std::size_t xsi_message_queue::get_mem_size
   (std::size_t max_msg_size, std::size_t max_num_msg)
{  return detail::mq_hdr_t::get_mem_size(max_msg_size, max_num_msg) 
	  - detail::managed_open_or_create_impl<shared_memory_object>::ManagedOpenOrCreateUserOffset
          + detail::managed_open_or_create_impl<xsi_shared_memory_file_wrapper,false,true>::ManagedOpenOrCreateUserOffset;
}

inline xsi_message_queue::xsi_message_queue(create_only_t create_only,
				    xsi_key key,
                                    std::size_t max_num_msg, 
                                    std::size_t max_msg_size,
                                    const permissions &perm)
      //Create shared memory and execute functor atomically
   :  m_shmem(create_only,
              key, 
              get_mem_size(max_msg_size, max_num_msg),
	      read_write,
	      static_cast<void*>(0),
              //Prepare initialization functor
              detail::initialization_func_t (max_num_msg, max_msg_size),
              perm)
{}

inline xsi_message_queue::xsi_message_queue(open_or_create_t open_or_create,
				    xsi_key key,
                                    std::size_t max_num_msg, 
                                    std::size_t max_msg_size,
                                    const permissions &perm)
      //Create shared memory and execute functor atomically
   :  m_shmem(open_or_create, 
              key, 
              get_mem_size(max_msg_size, max_num_msg),
              read_write,
              static_cast<void*>(0),
              //Prepare initialization functor
              detail::initialization_func_t (max_num_msg, max_msg_size),
              perm)
{}

inline xsi_message_queue::xsi_message_queue(open_only_t open_only,
                			   xsi_key key) 
   //Create shared memory and execute functor atomically
   :  m_shmem(open_only, 
              key,
              read_write,
              static_cast<void*>(0),
              //Prepare initialization functor
              detail::initialization_func_t ())
{}

inline void xsi_message_queue::send
   (const void *buffer, std::size_t buffer_size, unsigned int priority)
{  this->do_send(blocking, buffer, buffer_size, priority, ptime()); }

inline bool xsi_message_queue::try_send
   (const void *buffer, std::size_t buffer_size, unsigned int priority)
{  return this->do_send(non_blocking, buffer, buffer_size, priority, ptime()); }

inline bool xsi_message_queue::timed_send
   (const void *buffer, std::size_t buffer_size
   ,unsigned int priority, const boost::posix_time::ptime &abs_time)
{
   if(abs_time == boost::posix_time::pos_infin){
      this->send(buffer, buffer_size, priority);
      return true;
   }
   return this->do_send(timed, buffer, buffer_size, priority, abs_time);
}

inline bool xsi_message_queue::do_send(block_t block,
                                const void *buffer,      std::size_t buffer_size, 
                                unsigned int priority,   const boost::posix_time::ptime &abs_time)
{
   detail::mq_hdr_t *p_hdr = static_cast<detail::mq_hdr_t*>(m_shmem.get_user_address());
   //Check if buffer is smaller than maximum allowed
   if (buffer_size > p_hdr->m_max_msg_size) {
      throw interprocess_exception(size_error);
   }

   //---------------------------------------------
   scoped_lock<interprocess_mutex> lock(p_hdr->m_mutex);
   //---------------------------------------------
   {
      //If the queue is full execute blocking logic
      if (p_hdr->is_full()) {

         switch(block){
            case non_blocking :
               return false;
            break;

            case blocking :
               do{
                  p_hdr->m_cond_send.wait(lock);
               }
               while (p_hdr->is_full());
            break;

            case timed :
               do{
                  if(!p_hdr->m_cond_send.timed_wait(lock, abs_time)){
                     if(p_hdr->is_full())
                        return false;
                     break;
                  }
               }
               while (p_hdr->is_full());
            break;
            default:
            break;
         }
      }
      
      //Get the first free message from free message queue
      detail::msg_hdr_t *free_msg = p_hdr->free_msg();
      if (free_msg == 0) {
         throw interprocess_exception("boost::interprocess::xsi_message_queue corrupted");
      }

      //Copy control data to the free message
      free_msg->priority = priority;
      free_msg->len      = buffer_size;

      //Copy user buffer to the message
      std::memcpy(free_msg->data(), buffer, buffer_size);

//      bool was_empty = p_hdr->is_empty();
      //Insert the first free message in the priority queue
      p_hdr->queue_free_msg();
      
      //If this message changes the queue empty state, notify it to receivers
//      if (was_empty){
         p_hdr->m_cond_recv.notify_one();
//      }
   }  // Lock end

   return true;
}

inline void xsi_message_queue::receive(void *buffer,              std::size_t buffer_size, 
                                   std::size_t &recvd_size,   unsigned int &priority)
{  this->do_receive(blocking, buffer, buffer_size, recvd_size, priority, ptime()); }

inline bool
   xsi_message_queue::try_receive(void *buffer,              std::size_t buffer_size, 
                              std::size_t &recvd_size,   unsigned int &priority)
{  return this->do_receive(non_blocking, buffer, buffer_size, recvd_size, priority, ptime()); }

inline bool
   xsi_message_queue::timed_receive(void *buffer,              std::size_t buffer_size, 
                                std::size_t &recvd_size,   unsigned int &priority,
                                const boost::posix_time::ptime &abs_time)
{
   if(abs_time == boost::posix_time::pos_infin){
      this->receive(buffer, buffer_size, recvd_size, priority);
      return true;
   }
   return this->do_receive(timed, buffer, buffer_size, recvd_size, priority, abs_time);
}

inline bool
   xsi_message_queue::do_receive(block_t block,
                          void *buffer,              std::size_t buffer_size, 
                          std::size_t &recvd_size,   unsigned int &priority,
                          const boost::posix_time::ptime &abs_time)
{
   detail::mq_hdr_t *p_hdr = static_cast<detail::mq_hdr_t*>(m_shmem.get_user_address());
   //Check if buffer is big enough for any message
   if (buffer_size < p_hdr->m_max_msg_size) {
      throw interprocess_exception(size_error);
   }

   //---------------------------------------------
   scoped_lock<interprocess_mutex> lock(p_hdr->m_mutex);
   //---------------------------------------------
   {
      //If there are no messages execute blocking logic
      if (p_hdr->is_empty()) {
         switch(block){
            case non_blocking :
               return false;
            break;

            case blocking :
               do{
                  p_hdr->m_cond_recv.wait(lock);
               }
               while (p_hdr->is_empty());
            break;

            case timed :
               do{
                  if(!p_hdr->m_cond_recv.timed_wait(lock, abs_time)){
                     if(p_hdr->is_empty())
                        return false;
                     break;
                  }
               }
               while (p_hdr->is_empty());
            break;

            //Paranoia check
            default:
            break;
         }
      }

      //Thre is at least message ready to pick, get the top one
      detail::msg_hdr_t *top_msg = p_hdr->top_msg();

      //Paranoia check
      if (top_msg == 0) {
         throw interprocess_exception("boost::interprocess::message_queue corrupted");
      }

      //Get data from the message
      recvd_size     = top_msg->len;
      priority       = top_msg->priority;

      //Copy data to receiver's bufers
      std::memcpy(buffer, top_msg->data(), recvd_size);

//      bool was_full = p_hdr->is_full();

      //Free top message and put it in the free message list
      p_hdr->free_top_msg();

      //If this reception changes the queue full state, notify senders
//      if (was_full){
         p_hdr->m_cond_send.notify_one();
//      }
   }  //Lock end

   return true;
}

inline std::size_t xsi_message_queue::get_max_msg() const
{  
   detail::mq_hdr_t *p_hdr = static_cast<detail::mq_hdr_t*>(m_shmem.get_user_address());
   return p_hdr ? p_hdr->m_max_num_msg : 0;  }

inline std::size_t xsi_message_queue::get_max_msg_size() const
{  
   detail::mq_hdr_t *p_hdr = static_cast<detail::mq_hdr_t*>(m_shmem.get_user_address());
   return p_hdr ? p_hdr->m_max_msg_size : 0;  
}

inline std::size_t xsi_message_queue::get_num_msg()
{  
   detail::mq_hdr_t *p_hdr = static_cast<detail::mq_hdr_t*>(m_shmem.get_user_address());
   if(p_hdr){
      //---------------------------------------------
      scoped_lock<interprocess_mutex> lock(p_hdr->m_mutex);
      //---------------------------------------------
      return p_hdr->m_cur_num_msg;
   }

   return 0;  
}

inline bool xsi_message_queue::remove(xsi_key key)
{
	int id = shmget(key.get_key(), 0, read_only);
	if(id != -1) 
		return xsi_shared_memory::remove(id);  
	else
		return false;
}


/// @endcond

}} //namespace boost{  namespace interprocess{

#include <boost/interprocess/detail/config_end.hpp>

#endif   //#ifndef BOOST_INTERPROCESS_MESSAGE_QUEUE_HPP
